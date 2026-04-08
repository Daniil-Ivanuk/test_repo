import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_absolute_error, mean_squared_error
from statsmodels.tsa.statespace.sarimax import SARIMAX
import os
import warnings
warnings.filterwarnings('ignore')

# ============================================
# 0. НАСТРОЙКИ
# ============================================

output_folder = 'graphs'
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

FORECAST_END_DATE = pd.Timestamp('2026-11-01')

# ============================================
# 1. ЗАГРУЗКА ДАННЫХ
# ============================================

df = pd.read_excel('finish.xlsx')

df['Дата'] = pd.to_datetime(df['Дата'])
df = df.sort_values('Дата').reset_index(drop=True)

last_date = df['Дата'].max()
print(f"Последняя дата в данных: {last_date.strftime('%Y-%m')}")
print(f"Прогноз до: {FORECAST_END_DATE.strftime('%Y-%m')}")

forecast_horizon = (FORECAST_END_DATE.year - last_date.year) * 12 + (FORECAST_END_DATE.month - last_date.month)
print(f"Количество месяцев прогноза: {forecast_horizon}")

# ============================================
# 2. ВЫБОР ПОКАЗАТЕЛЕЙ
# ============================================

targets = {
    'Всего преступлений': 'Всего преступлений, совершенных с использованием информационно-телекоммуникационных технологий или в сфере компьютерной информации',
    'Мошенничества': 'мошенничества',
    'Мошенничество в сфере компьютерной информации': 'мошенничество в сфере компьютерной информации',
    'Публичные призывы (терроризм)': 'публичные призывы к осуществлению террористической деятельности',
    'Публичные призывы (экстремизм)': 'публичные призывы к осуществлению экстремистской деятельности',
    'Преступления в сфере компьютерной информации': 'преступления в сфере компьютерной информации',
    'Неправомерный доступ': 'неправомерный доступ к компьютерной информации',
    'Создание и распространение вредоносного ПО': 'создание, использование и распространение вредоносных компьютерных программ'
}

series_dict = {}
for name, col_name in targets.items():
    mask = df['Категория'].str.contains(col_name, na=False) | df['Подкатегория'].str.contains(col_name, na=False)
    
    if mask.any():
        series = df.loc[mask, ['Дата', 'Всего']].copy()
        series = series.set_index('Дата').sort_index()
        series_dict[name] = series['Всего']
        print(f"Найден показатель: {name}")
    else:
        print(f"Предупреждение: не найден показатель '{name}'")

# ============================================
# 3. ЛИНЕЙНАЯ РЕГРЕССИЯ
# ============================================

def linear_forecast(series, forecast_horizon):
    X = np.arange(len(series)).reshape(-1, 1)
    y = series.values
    
    model = LinearRegression()
    model.fit(X, y)
    
    X_future = np.arange(len(series), len(series) + forecast_horizon).reshape(-1, 1)
    y_pred = model.predict(X_future)
    
    future_dates = pd.date_range(start=series.index[-1] + pd.DateOffset(months=1), periods=forecast_horizon, freq='MS')
    
    y_pred_train = model.predict(X)
    rmse = np.sqrt(mean_squared_error(y, y_pred_train))
    mae = mean_absolute_error(y, y_pred_train)
    
    return {
        'forecast': y_pred,
        'future_dates': future_dates,
        'rmse': rmse,
        'mae': mae,
        'r2': model.score(X, y),
        'slope': model.coef_[0],
        'intercept': model.intercept_,
        'success': True
    }

# ============================================
# 4. SARIMA
# ============================================

def sarima_forecast(series, forecast_horizon):
    y = series.values.astype(float)
    
    best_aic = np.inf
    best_params = None
    best_model = None
    
    p_values = range(0, 3)
    d_values = [0, 1]
    q_values = range(0, 3)
    P_values = [0, 1]
    D_values = [0, 1]
    Q_values = [0, 1]
    s = 12
    y_log = np.log1p(y)
    print("    Поиск лучших параметров SARIMA...")
    for p in p_values:
        for d in d_values:
            for q in q_values:
                for P in P_values:
                    for D in D_values:
                        for Q in Q_values:
                            try:
                                model = SARIMAX(y_log, 
                                              order=(p, d, q),
                                              seasonal_order=(P, D, Q, s),
                                              simple_differencing=False)
                                fitted = model.fit(disp=False)
                                if fitted.aic < best_aic:
                                    best_aic = fitted.aic
                                    best_params = (p, d, q, P, D, Q, s)
                                    best_model = fitted
                                    print(f"      Найдена модель с AIC={best_aic:.0f}")
                            except:
                                continue
    
    if best_model is None:
        print("    SARIMA не найдена, использую ARIMA(1,1,1)")
        from statsmodels.tsa.arima.model import ARIMA
        model = ARIMA(y, order=(1, 1, 1))
        best_model = model.fit()
        best_params = (1, 1, 1, 0, 0, 0, 0)
        best_aic = best_model.aic
    
    try:
        forecast_log = best_model.forecast(steps=forecast_horizon)
        forecast = np.expm1(forecast_log)
        forecast = np.maximum(forecast, 0)
        y_pred_train = np.expm1(best_model.fittedvalues) 
        valid_idx = ~np.isnan(y_pred_train)
        rmse = np.sqrt(mean_squared_error(y[valid_idx], y_pred_train[valid_idx]))
        mae = mean_absolute_error(y[valid_idx], y_pred_train[valid_idx])
        
        future_dates = pd.date_range(start=series.index[-1] + pd.DateOffset(months=1), periods=forecast_horizon, freq='MS')
        
        return {
            'forecast': forecast.values if hasattr(forecast, 'values') else np.array(forecast),
            'future_dates': future_dates,
            'rmse': rmse,
            'mae': mae,
            'params': best_params,
            'aic': best_aic,
            'model': best_model,
            'success': True
        }
    except Exception as e:
        print(f"    Ошибка SARIMA: {e}")
        return {'success': False}

# ============================================
# 5. ОЧИСТКА ДАННЫХ ДЛЯ КОРРЕЛЯЦИИ
# ============================================

def make_stationary(series):
    y = series.values
    y_diff = np.diff(y)
    if len(y_diff) > 12:
        y_stationary = np.diff(y_diff, 12)
    else:
        y_stationary = y_diff
    return pd.Series(y_stationary, name=series.name)

# ============================================
# 6. ПОСТРОЕНИЕ ГРАФИКА
# ============================================

def plot_forecast(series, name, linear_result, sarima_result, forecast_horizon, save_folder='graphs'):
    plt.figure(figsize=(14, 6))
    
    # Фактические данные
    plt.plot(series.index, series.values, 'o-', label='Фактические данные', markersize=4, color='black', linewidth=1.5)
    
    # Линейная регрессия
    X = np.arange(len(series)).reshape(-1, 1)
    y_trend = linear_result['slope'] * X.flatten() + linear_result['intercept']
    plt.plot(series.index, y_trend, '--', label=f'Линейный тренд (R²={linear_result["r2"]:.3f})', color='blue', linewidth=2)
    
    # Прогноз линейной регрессии
    plt.plot(linear_result['future_dates'], linear_result['forecast'], 's-', label='Прогноз (линейная)', color='blue', markersize=4, linewidth=1.5, alpha=0.7)
    
    # SARIMA (если успешно)
    if sarima_result.get('success', False):
    # Обратное преобразование fitted values из логарифмической шкалы
        y_sarima_fitted = np.expm1(sarima_result['model'].fittedvalues)
        
        params_str = f"({sarima_result['params'][0]},{sarima_result['params'][1]},{sarima_result['params'][2]})"
        seasonal_str = f"({sarima_result['params'][3]},{sarima_result['params'][4]},{sarima_result['params'][5]},{sarima_result['params'][6]})"
        plt.plot(series.index, y_sarima_fitted, '--', label=f'SARIMA{params_str} x {seasonal_str} (AIC={sarima_result["aic"]:.0f})', color='green', linewidth=2)
    
        plt.plot(sarima_result['future_dates'], sarima_result['forecast'], '^-', label='Прогноз (SARIMA)', color='red', markersize=5, linewidth=1.5)
    
    plt.axvline(x=series.index[-1], color='gray', linestyle=':', alpha=0.7, label='Начало прогноза')
    
    plt.title(f'{name}\nПрогноз до ноября 2026 г.', fontsize=14)
    plt.xlabel('Дата', fontsize=12)
    plt.ylabel('Количество преступлений', fontsize=12)
    plt.legend(fontsize=9)
    plt.grid(True, alpha=0.3)
    plt.xticks(rotation=45)
    plt.tight_layout()
    
    safe_name = name.replace(' ', '_').replace('(', '').replace(')', '').replace('/', '_')
    filename = os.path.join(save_folder, f'{safe_name}_forecast_2026.png')
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    print(f"  Сохранён график: {filename}")
    
    plt.show()

# ============================================
# 7. ОСНОВНОЙ ЦИКЛ
# ============================================

print("\n" + "=" * 60)
print("ПОСТРОЕНИЕ МОДЕЛЕЙ И ПРОГНОЗ ДО НОЯБРЯ 2026")
print("=" * 60)

results = []

for name, series in series_dict.items():
    print(f"\n--- {name} ---")
    
    print("  Линейная регрессия...")
    linear_result = linear_forecast(series, forecast_horizon)
    
    print("  SARIMA...")
    sarima_result = sarima_forecast(series, forecast_horizon)
    
    plot_forecast(series, name, linear_result, sarima_result, forecast_horizon, output_folder)
    
    results.append({
        'Показатель': name,
        'Линейная_R2': linear_result['r2'],
        'Линейная_RMSE': linear_result['rmse'],
        'Линейная_MAE': linear_result['mae'],
        'SARIMA_RMSE': sarima_result['rmse'] if sarima_result.get('success') else None,
        'SARIMA_MAE': sarima_result['mae'] if sarima_result.get('success') else None,
        'SARIMA_params': str(sarima_result['params']) if sarima_result.get('success') else 'Ошибка',
        'SARIMA_AIC': sarima_result['aic'] if sarima_result.get('success') else None
    })

# ============================================
# 8. ТАБЛИЦА ПРОГНОЗОВ
# ============================================

print("\n" + "=" * 60)
print("ТАБЛИЦА ПРОГНОЗОВ ДО НОЯБРЯ 2026")
print("=" * 60)

future_dates = pd.date_range(start=last_date + pd.DateOffset(months=1), periods=forecast_horizon, freq='MS')

forecast_linear = {}
forecast_sarima = {}

for name, series in series_dict.items():
    linear_result = linear_forecast(series, forecast_horizon)
    sarima_result = sarima_forecast(series, forecast_horizon)
    forecast_linear[name] = linear_result['forecast']
    if sarima_result.get('success'):
        forecast_sarima[name] = sarima_result['forecast']

print("\nЛИНЕЙНАЯ РЕГРЕССИЯ:")
linear_df = pd.DataFrame(forecast_linear, index=future_dates).round(0).astype(int)
print(linear_df.to_string())

if forecast_sarima:
    print("\nSARIMA (с учётом сезонности):")
    sarima_df = pd.DataFrame(forecast_sarima, index=future_dates).round(0).astype(int)
    print(sarima_df.to_string())
    sarima_df.to_csv(os.path.join(output_folder, 'forecast_sarima_2026.csv'))

linear_df.to_csv(os.path.join(output_folder, 'forecast_linear_2026.csv'))

# ============================================
# 9. СРАВНЕНИЕ МОДЕЛЕЙ
# ============================================

results_df = pd.DataFrame(results)
results_df.to_csv(os.path.join(output_folder, 'model_comparison.csv'), index=False)
print(f"\nТаблица сравнения сохранена: {os.path.join(output_folder, 'model_comparison.csv')}")

# ============================================
# 10. КОРРЕЛЯЦИОННАЯ МАТРИЦА
# ============================================

stationary_series = {}
for name, series in series_dict.items():
    stationary_series[name] = make_stationary(series)

stationary_df = pd.DataFrame(stationary_series).dropna()
corr_matrix = stationary_df.corr()

plt.figure(figsize=(10, 8))
sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0, fmt='.2f', square=True)
plt.title('Корреляционная матрица показателей\n(после удаления тренда и сезонности)', fontsize=14)
plt.tight_layout()

corr_file = os.path.join(output_folder, 'correlation_matrix_stationary.png')
plt.savefig(corr_file, dpi=150, bbox_inches='tight')
print(f"\nСохранена корреляционная матрица: {corr_file}")
plt.show()

print("\n" + "=" * 60)
print(f"ГОТОВО! Все файлы сохранены в: {os.path.abspath(output_folder)}")
print("=" * 60)
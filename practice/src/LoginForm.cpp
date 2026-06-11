#include "../include/LoginForm.h"
#include "../include/RegisterForm.h"

LoginForm::LoginForm()
{
    is_connected = false;
    InitializeComponent();
}

void LoginForm::InitializeComponent()
{
    this->Text = gcnew System::String(L"Система учёта рабочего времени - Подключение к базе данных");
    this->Size = System::Drawing::Size(450, 350);
    this->StartPosition = FormStartPosition::CenterScreen;
    this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
    this->MaximizeBox = false;

    Label^ titleLabel = gcnew Label();
    titleLabel->Text = gcnew System::String(L"Система учёта рабочего времени");
    titleLabel->SetBounds(20, 10, 400, 25);
    titleLabel->Font = gcnew System::Drawing::Font(L"Segoe UI", 12, FontStyle::Bold);
    titleLabel->TextAlign = ContentAlignment::MiddleCenter;
    this->Controls->Add(titleLabel);

    Label^ serverLabel = gcnew Label();
    serverLabel->Text = gcnew System::String(L"Сервер:");
    serverLabel->SetBounds(20, 50, 100, 20);
    this->Controls->Add(serverLabel);

    serverTextBox = gcnew TextBox();
    serverTextBox->SetBounds(130, 50, 280, 20);
    serverTextBox->Text = L"WIN-289CA8726EM";
    this->Controls->Add(serverTextBox);

    Label^ databaseLabel = gcnew Label();
    databaseLabel->Text = gcnew System::String(L"База данных:");
    databaseLabel->SetBounds(20, 80, 100, 20);
    this->Controls->Add(databaseLabel);

    databaseTextBox = gcnew TextBox();
    databaseTextBox->SetBounds(130, 80, 280, 20);
    databaseTextBox->Text = L"TimeTracking";
    this->Controls->Add(databaseTextBox);

    integratedAuthCheckBox = gcnew CheckBox();
    integratedAuthCheckBox->Text = gcnew System::String(L"Windows аутентификация");
    integratedAuthCheckBox->SetBounds(20, 110, 200, 20);
    integratedAuthCheckBox->Checked = true;
    integratedAuthCheckBox->CheckedChanged += gcnew EventHandler(this, &LoginForm::integratedAuthCheckBox_CheckedChanged);
    this->Controls->Add(integratedAuthCheckBox);

    Label^ usernameLabel = gcnew Label();
    usernameLabel->Text = gcnew System::String(L"Пользователь:");
    usernameLabel->SetBounds(20, 140, 100, 20);
    this->Controls->Add(usernameLabel);

    usernameTextBox = gcnew TextBox();
    usernameTextBox->SetBounds(130, 140, 280, 20);
    usernameTextBox->Enabled = false;
    this->Controls->Add(usernameTextBox);

    Label^ passwordLabel = gcnew Label();
    passwordLabel->Text = gcnew System::String(L"Пароль:");
    passwordLabel->SetBounds(20, 170, 100, 20);
    this->Controls->Add(passwordLabel);

    passwordTextBox = gcnew TextBox();
    passwordTextBox->SetBounds(130, 170, 280, 20);
    passwordTextBox->PasswordChar = '*';
    passwordTextBox->Enabled = false;
    this->Controls->Add(passwordTextBox);

    connectButton = gcnew Button();
    connectButton->Text = gcnew System::String(L"Подключиться");
    connectButton->SetBounds(130, 220, 120, 35);
    connectButton->BackColor = Color::LightBlue;
    connectButton->FlatStyle = FlatStyle::Flat;
    connectButton->Click += gcnew EventHandler(this, &LoginForm::connectButton_Click);
    this->Controls->Add(connectButton);

    registerButton = gcnew Button();
    registerButton->Text = gcnew System::String(L"Регистрация сотрудника");
    registerButton->SetBounds(260, 220, 150, 35);
    registerButton->BackColor = Color::LightGreen;
    registerButton->FlatStyle = FlatStyle::Flat;
    registerButton->Click += gcnew EventHandler(this, &LoginForm::registerButton_Click);
    this->Controls->Add(registerButton);

    Label^ infoLabel = gcnew Label();
    infoLabel->Text = gcnew System::String(L"Подключитесь к базе данных для доступа к системе");
    infoLabel->SetBounds(20, 270, 400, 40);
    infoLabel->TextAlign = ContentAlignment::MiddleCenter;
    infoLabel->ForeColor = Color::Gray;
    this->Controls->Add(infoLabel);
}

void LoginForm::integratedAuthCheckBox_CheckedChanged(Object^ sender, EventArgs^ args)
{
    bool isEnabled = !integratedAuthCheckBox->Checked;
    usernameTextBox->Enabled = isEnabled;
    passwordTextBox->Enabled = isEnabled;
}

void LoginForm::connectButton_Click(Object^ sender, EventArgs^ args)
{
    String^ connectionString;

    if (integratedAuthCheckBox->Checked)
    {
        connectionString = String::Format(
            L"Data Source={0};Initial Catalog={1};Integrated Security=True;Connect Timeout=30;",
            serverTextBox->Text, databaseTextBox->Text);
    }
    else
    {
        connectionString = String::Format(
            L"Data Source={0};Initial Catalog={1};User ID={2};Password={3};Connect Timeout=30;",
            serverTextBox->Text, databaseTextBox->Text, usernameTextBox->Text, passwordTextBox->Text);
    }

    try
    {
        sqlConnection = gcnew SqlConnection(connectionString);
        sqlConnection->Open();

        SqlCommand^ checkCommand = gcnew SqlCommand(
            L"SELECT COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = 'Employee'",
            sqlConnection);
        int tableCount = Convert::ToInt32(checkCommand->ExecuteScalar());

        if (tableCount == 0)
        {
            MessageBox::Show(
                gcnew System::String(L"База данных не содержит необходимых таблиц.\nВыполните скрипт TimeTracking.sql"),
                gcnew System::String(L"Предупреждение"),
                MessageBoxButtons::OK,
                MessageBoxIcon::Warning);
            sqlConnection->Close();
            return;
        }

        is_connected = true;

        MessageBox::Show(
            gcnew System::String(L"Подключение к базе данных успешно установлено!"),
            gcnew System::String(L"Успех"),
            MessageBoxButtons::OK,
            MessageBoxIcon::Information);

        this->Close();
    }
    catch (Exception^ e)
    {
        MessageBox::Show(
            gcnew System::String(L"Ошибка подключения к базе данных:\n") + e->Message,
            gcnew System::String(L"Ошибка"),
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
    }
}

void LoginForm::registerButton_Click(Object^ sender, EventArgs^ args)
{
    String^ connectionString;

    if (integratedAuthCheckBox->Checked)
    {
        connectionString = String::Format(
            L"Data Source={0};Initial Catalog={1};Integrated Security=True;Connect Timeout=30;",
            serverTextBox->Text, databaseTextBox->Text);
    }
    else
    {
        connectionString = String::Format(
            L"Data Source={0};Initial Catalog={1};User ID={2};Password={3};Connect Timeout=30;",
            serverTextBox->Text, databaseTextBox->Text, usernameTextBox->Text, passwordTextBox->Text);
    }

    try
    {
        SqlConnection^ tempConnection = gcnew SqlConnection(connectionString);
        tempConnection->Open();

        RegisterForm^ registerForm = gcnew RegisterForm(tempConnection);
        registerForm->ShowDialog();

        if (registerForm->IsRegistered)
        {
            sqlConnection = tempConnection;
            is_connected = true;

            MessageBox::Show(
                gcnew System::String(L"Регистрация успешно завершена! Теперь вы можете войти в систему."),
                gcnew System::String(L"Успех"),
                MessageBoxButtons::OK,
                MessageBoxIcon::Information);

            this->Close();
        }
        else
        {
            tempConnection->Close();
        }
    }
    catch (Exception^ e)
    {
        MessageBox::Show(
            gcnew System::String(L"Ошибка подключения к базе данных:\n") + e->Message,
            gcnew System::String(L"Ошибка"),
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
    }
}
#include "../include/MainFormAdmin.h"

MainFormAdmin::MainFormAdmin(SqlConnection^ connection)
{
    sqlConnection = connection;
    adminPassword = L"admin123";
    InitializeComponent();
    LoadEmployees();
    LoadEmployeeCombo();
    LoadTimeEntries();
}

void MainFormAdmin::InitializeComponent()
{
    this->Text = gcnew System::String(L"Панель администратора");
    this->Size = System::Drawing::Size(1200, 750);
    this->StartPosition = FormStartPosition::CenterScreen;
    this->WindowState = FormWindowState::Maximized;

    tabMain = gcnew TabControl();
    tabMain->Dock = DockStyle::Fill;
    this->Controls->Add(tabMain);

    tabEmployees = gcnew TabPage(gcnew System::String(L"Управление сотрудниками"));
    tabMain->TabPages->Add(tabEmployees);

    Panel^ panelButtons = gcnew Panel();
    panelButtons->SetBounds(10, 10, 1150, 45);
    tabEmployees->Controls->Add(panelButtons);

    btnAdd = gcnew Button();
    btnAdd->Text = gcnew System::String(L"Добавить сотрудника");
    btnAdd->SetBounds(10, 8, 150, 30);
    btnAdd->Click += gcnew EventHandler(this, &MainFormAdmin::OnAddClick);
    panelButtons->Controls->Add(btnAdd);

    btnEdit = gcnew Button();
    btnEdit->Text = gcnew System::String(L"Редактировать");
    btnEdit->SetBounds(170, 8, 120, 30);
    btnEdit->Enabled = false;
    btnEdit->Click += gcnew EventHandler(this, &MainFormAdmin::OnEditClick);
    panelButtons->Controls->Add(btnEdit);

    btnDelete = gcnew Button();
    btnDelete->Text = gcnew System::String(L"Удалить");
    btnDelete->SetBounds(300, 8, 120, 30);
    btnDelete->Enabled = false;
    btnDelete->Click += gcnew EventHandler(this, &MainFormAdmin::OnDeleteClick);
    panelButtons->Controls->Add(btnDelete);

    btnRefresh = gcnew Button();
    btnRefresh->Text = gcnew System::String(L"Обновить");
    btnRefresh->SetBounds(430, 8, 120, 30);
    btnRefresh->Click += gcnew EventHandler(this, &MainFormAdmin::OnRefreshClick);
    panelButtons->Controls->Add(btnRefresh);

    dgvEmployees = gcnew DataGridView();
    dgvEmployees->SetBounds(10, 60, 1150, 600);
    dgvEmployees->ReadOnly = true;
    dgvEmployees->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    dgvEmployees->MultiSelect = false;
    dgvEmployees->AllowUserToAddRows = false;
    dgvEmployees->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
    dgvEmployees->SelectionChanged += gcnew EventHandler(this, &MainFormAdmin::OnEmployeesSelectionChanged);
    tabEmployees->Controls->Add(dgvEmployees);

    tabTimeEntries = gcnew TabPage(gcnew System::String(L"Отметки времени"));
    tabMain->TabPages->Add(tabTimeEntries);

    Panel^ panelFilter = gcnew Panel();
    panelFilter->SetBounds(10, 10, 1150, 50);
    tabTimeEntries->Controls->Add(panelFilter);

    Label^ lblEmployee = gcnew Label();
    lblEmployee->Text = gcnew System::String(L"Сотрудник:");
    lblEmployee->SetBounds(10, 15, 80, 25);
    panelFilter->Controls->Add(lblEmployee);

    cmbEmployee = gcnew ComboBox();
    cmbEmployee->SetBounds(90, 13, 200, 25);
    cmbEmployee->DropDownStyle = ComboBoxStyle::DropDownList;
    panelFilter->Controls->Add(cmbEmployee);

    Label^ lblStart = gcnew Label();
    lblStart->Text = gcnew System::String(L"С даты:");
    lblStart->SetBounds(310, 15, 60, 25);
    panelFilter->Controls->Add(lblStart);

    dtpStartDate = gcnew DateTimePicker();
    dtpStartDate->SetBounds(370, 13, 120, 25);
    dtpStartDate->Format = DateTimePickerFormat::Short;
    dtpStartDate->Value = DateTime::Now.AddDays(-30);
    panelFilter->Controls->Add(dtpStartDate);

    Label^ lblEnd = gcnew Label();
    lblEnd->Text = gcnew System::String(L"По дату:");
    lblEnd->SetBounds(510, 15, 60, 25);
    panelFilter->Controls->Add(lblEnd);

    dtpEndDate = gcnew DateTimePicker();
    dtpEndDate->SetBounds(570, 13, 120, 25);
    dtpEndDate->Format = DateTimePickerFormat::Short;
    dtpEndDate->Value = DateTime::Now;
    panelFilter->Controls->Add(dtpEndDate);

    btnFilter = gcnew Button();
    btnFilter->Text = gcnew System::String(L"Показать");
    btnFilter->SetBounds(710, 11, 100, 28);
    btnFilter->Click += gcnew EventHandler(this, &MainFormAdmin::OnFilterClick);
    panelFilter->Controls->Add(btnFilter);

    btnCheckIn = gcnew Button();
    btnCheckIn->Text = gcnew System::String(L"Отметить приход");
    btnCheckIn->SetBounds(830, 11, 130, 28);
    btnCheckIn->Click += gcnew EventHandler(this, &MainFormAdmin::OnCheckInClick);
    panelFilter->Controls->Add(btnCheckIn);

    btnCheckOut = gcnew Button();
    btnCheckOut->Text = gcnew System::String(L"Отметить уход");
    btnCheckOut->SetBounds(970, 11, 130, 28);
    btnCheckOut->Click += gcnew EventHandler(this, &MainFormAdmin::OnCheckOutClick);
    panelFilter->Controls->Add(btnCheckOut);

    lblLastEntry = gcnew Label();
    lblLastEntry->Text = gcnew System::String(L"Последняя отметка: ---");
    lblLastEntry->SetBounds(10, 65, 500, 25);
    tabTimeEntries->Controls->Add(lblLastEntry);

    dgvTimeEntries = gcnew DataGridView();
    dgvTimeEntries->SetBounds(10, 95, 1150, 565);
    dgvTimeEntries->ReadOnly = true;
    dgvTimeEntries->AllowUserToAddRows = false;
    dgvTimeEntries->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
    tabTimeEntries->Controls->Add(dgvTimeEntries);

    tabReports = gcnew TabPage(gcnew System::String(L"Формирование отчётов"));
    tabMain->TabPages->Add(tabReports);

    Panel^ panelReport = gcnew Panel();
    panelReport->SetBounds(10, 10, 1150, 50);
    tabReports->Controls->Add(panelReport);

    Label^ lblReportStart = gcnew Label();
    lblReportStart->Text = gcnew System::String(L"Начало периода:");
    lblReportStart->SetBounds(10, 15, 100, 25);
    panelReport->Controls->Add(lblReportStart);

    dtpReportStart = gcnew DateTimePicker();
    dtpReportStart->SetBounds(115, 13, 120, 25);
    dtpReportStart->Format = DateTimePickerFormat::Short;
    dtpReportStart->Value = DateTime(DateTime::Now.Year, DateTime::Now.Month, 1);
    panelReport->Controls->Add(dtpReportStart);

    Label^ lblReportEnd = gcnew Label();
    lblReportEnd->Text = gcnew System::String(L"Конец периода:");
    lblReportEnd->SetBounds(250, 15, 100, 25);
    panelReport->Controls->Add(lblReportEnd);

    dtpReportEnd = gcnew DateTimePicker();
    dtpReportEnd->SetBounds(355, 13, 120, 25);
    dtpReportEnd->Format = DateTimePickerFormat::Short;
    dtpReportEnd->Value = DateTime::Now;
    panelReport->Controls->Add(dtpReportEnd);

    btnGenerateReport = gcnew Button();
    btnGenerateReport->Text = gcnew System::String(L"Сформировать отчёт");
    btnGenerateReport->SetBounds(500, 11, 150, 28);
    btnGenerateReport->Click += gcnew EventHandler(this, &MainFormAdmin::OnGenerateReportClick);
    panelReport->Controls->Add(btnGenerateReport);

    btnExportCsv = gcnew Button();
    btnExportCsv->Text = gcnew System::String(L"Экспорт в CSV");
    btnExportCsv->SetBounds(660, 11, 120, 28);
    btnExportCsv->Click += gcnew EventHandler(this, &MainFormAdmin::OnExportCsvClick);
    panelReport->Controls->Add(btnExportCsv);

    dgvReport = gcnew DataGridView();
    dgvReport->SetBounds(10, 65, 1150, 595);
    dgvReport->ReadOnly = true;
    dgvReport->AllowUserToAddRows = false;
    dgvReport->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
    tabReports->Controls->Add(dgvReport);

    saveFileDialog = gcnew SaveFileDialog();
    saveFileDialog->Filter = L"CSV файлы (*.csv)|*.csv";
    saveFileDialog->DefaultExt = L"csv";
}

// ... остальные методы без изменений (LoadEmployees, LoadEmployeeCombo, и т.д.) ...

// Только методы с MessageBox нужно исправить:

void MainFormAdmin::LoadEmployees()
{
    try
    {
        SqlCommand^ command = gcnew SqlCommand(
            L"SELECT e.Employee_ID, e.Last_Name, e.First_Name, e.Middle_Name, "
            L"p.Position_Name AS Position, e.Login_Name, e.Role_Name, e.Hire_Date, e.Is_Active "
            L"FROM Employee e INNER JOIN Position p ON e.Position_ID = p.Position_ID ORDER BY e.Last_Name, e.First_Name",
            sqlConnection);

        SqlDataAdapter^ adapter = gcnew SqlDataAdapter(command);
        DataTable^ dataTable = gcnew DataTable();
        adapter->Fill(dataTable);

        dgvEmployees->DataSource = dataTable;

        if (dgvEmployees->Columns->Count > 0)
        {
            dgvEmployees->Columns["Employee_ID"]->HeaderText = L"ID";
            dgvEmployees->Columns["Last_Name"]->HeaderText = L"Фамилия";
            dgvEmployees->Columns["First_Name"]->HeaderText = L"Имя";
            dgvEmployees->Columns["Middle_Name"]->HeaderText = L"Отчество";
            dgvEmployees->Columns["Position"]->HeaderText = L"Должность";
            dgvEmployees->Columns["Login_Name"]->HeaderText = L"Логин";
            dgvEmployees->Columns["Role_Name"]->HeaderText = L"Роль";
            dgvEmployees->Columns["Hire_Date"]->HeaderText = L"Дата приёма";
            dgvEmployees->Columns["Is_Active"]->HeaderText = L"Активен";
        }
    }
    catch (Exception^ ex)
    {
        MessageBox::Show(gcnew System::String(L"Ошибка загрузки сотрудников: ") + ex->Message);
    }
}

void MainFormAdmin::LoadEmployeeCombo()
{
    try
    {
        SqlCommand^ command = gcnew SqlCommand(
            L"SELECT Employee_ID, Last_Name + ' ' + First_Name AS FullName FROM Employee WHERE Is_Active = 1",
            sqlConnection);

        SqlDataReader^ reader = command->ExecuteReader();
        cmbEmployee->Items->Clear();

        while (reader->Read())
        {
            String^ item = reader["Employee_ID"]->ToString() + L" - " + reader["FullName"]->ToString();
            cmbEmployee->Items->Add(item);
        }
        reader->Close();

        if (cmbEmployee->Items->Count > 0) cmbEmployee->SelectedIndex = 0;
    }
    catch (Exception^ ex)
    {
        MessageBox::Show(gcnew System::String(L"Ошибка загрузки списка: ") + ex->Message);
    }
}

void MainFormAdmin::LoadTimeEntries()
{
    if (cmbEmployee->SelectedIndex == -1) return;

    try
    {
        String^ selectedItem = cmbEmployee->SelectedItem->ToString();
        int employeeId = Int32::Parse(selectedItem->Substring(0, selectedItem->IndexOf(L" -")));

        SqlCommand^ command = gcnew SqlCommand(
            L"SELECT TimeEntry_ID, Entry_Type, Entry_Time, "
            L"FORMAT(Entry_Time, 'dd.MM.yyyy') AS EntryDate, "
            L"FORMAT(Entry_Time, 'HH:mm:ss') AS EntryTimeOnly "
            L"FROM TimeEntry WHERE Employee_ID = @EmployeeID "
            L"AND CAST(Entry_Time AS DATE) BETWEEN @StartDate AND @EndDate "
            L"ORDER BY Entry_Time DESC", sqlConnection);

        command->Parameters->AddWithValue(L"@EmployeeID", employeeId);
        command->Parameters->AddWithValue(L"@StartDate", dtpStartDate->Value.ToString(L"yyyy-MM-dd"));
        command->Parameters->AddWithValue(L"@EndDate", dtpEndDate->Value.ToString(L"yyyy-MM-dd"));

        SqlDataAdapter^ adapter = gcnew SqlDataAdapter(command);
        DataTable^ dataTable = gcnew DataTable();
        adapter->Fill(dataTable);

        dgvTimeEntries->DataSource = dataTable;

        if (dgvTimeEntries->Columns->Count > 0)
        {
            dgvTimeEntries->Columns["TimeEntry_ID"]->Visible = false;
            dgvTimeEntries->Columns["Entry_Type"]->HeaderText = L"Тип";
            dgvTimeEntries->Columns["EntryDate"]->HeaderText = L"Дата";
            dgvTimeEntries->Columns["EntryTimeOnly"]->HeaderText = L"Время";
            dgvTimeEntries->Columns["Entry_Time"]->Visible = false;
        }

        UpdateLastEntryInfo();
    }
    catch (Exception^ ex)
    {
        MessageBox::Show(gcnew System::String(L"Ошибка загрузки отметок: ") + ex->Message);
    }
}

void MainFormAdmin::UpdateLastEntryInfo()
{
    if (cmbEmployee->SelectedIndex == -1) return;

    try
    {
        String^ selectedItem = cmbEmployee->SelectedItem->ToString();
        int employeeId = Int32::Parse(selectedItem->Substring(0, selectedItem->IndexOf(L" -")));

        SqlCommand^ command = gcnew SqlCommand(
            L"SELECT TOP 1 Entry_Type, Entry_Time, FORMAT(Entry_Time, 'dd.MM.yyyy HH:mm:ss') AS FormattedTime "
            L"FROM TimeEntry WHERE Employee_ID = @EmployeeID ORDER BY Entry_Time DESC", sqlConnection);
        command->Parameters->AddWithValue(L"@EmployeeID", employeeId);

        SqlDataReader^ reader = command->ExecuteReader();

        if (reader->Read())
        {
            String^ entryType = reader["Entry_Type"]->ToString();
            String^ formattedTime = reader["FormattedTime"]->ToString();
            String^ typeText = (entryType == L"CheckIn") ? L"ПРИХОД" : L"УХОД";
            lblLastEntry->Text = gcnew System::String(L"Последняя отметка: ") + typeText + L" - " + formattedTime;
            lblLastEntry->ForeColor = (entryType == L"CheckIn") ? Color::Green : Color::Red;
        }
        else
        {
            lblLastEntry->Text = gcnew System::String(L"Последняя отметка: нет данных");
            lblLastEntry->ForeColor = Color::Gray;
        }
        reader->Close();
    }
    catch (Exception^)
    {
        lblLastEntry->Text = gcnew System::String(L"Последняя отметка: ошибка");
    }
}

void MainFormAdmin::LoadReport()
{
    try
    {
        SqlCommand^ command = gcnew SqlCommand(
            L"SELECT e.Employee_ID, e.Last_Name + ' ' + e.First_Name + ISNULL(' ' + e.Middle_Name, '') AS FullName, "
            L"p.Position_Name AS Position, CAST(te.Entry_Time AS DATE) AS WorkDate, "
            L"MAX(CASE WHEN te.Entry_Type = 'CheckIn' THEN te.Entry_Time END) AS CheckInTime, "
            L"MAX(CASE WHEN te.Entry_Type = 'CheckOut' THEN te.Entry_Time END) AS CheckOutTime "
            L"FROM Employee e INNER JOIN Position p ON e.Position_ID = p.Position_ID "
            L"INNER JOIN TimeEntry te ON e.Employee_ID = te.Employee_ID "
            L"WHERE CAST(te.Entry_Time AS DATE) BETWEEN @StartDate AND @EndDate AND e.Is_Active = 1 "
            L"GROUP BY e.Employee_ID, e.Last_Name, e.First_Name, e.Middle_Name, p.Position_Name, CAST(te.Entry_Time AS DATE) "
            L"ORDER BY e.Last_Name, e.First_Name, WorkDate", sqlConnection);

        command->Parameters->AddWithValue(L"@StartDate", dtpReportStart->Value.ToString(L"yyyy-MM-dd"));
        command->Parameters->AddWithValue(L"@EndDate", dtpReportEnd->Value.ToString(L"yyyy-MM-dd"));

        SqlDataAdapter^ adapter = gcnew SqlDataAdapter(command);
        DataTable^ dataTable = gcnew DataTable();
        adapter->Fill(dataTable);

        dgvReport->DataSource = dataTable;

        if (dgvReport->Columns->Count > 0)
        {
            dgvReport->Columns["Employee_ID"]->HeaderText = L"ID";
            dgvReport->Columns["FullName"]->HeaderText = L"ФИО сотрудника";
            dgvReport->Columns["Position"]->HeaderText = L"Должность";
            dgvReport->Columns["WorkDate"]->HeaderText = L"Дата";
            dgvReport->Columns["CheckInTime"]->HeaderText = L"Время прихода";
            dgvReport->Columns["CheckOutTime"]->HeaderText = L"Время ухода";
        }
    }
    catch (Exception^ ex)
    {
        MessageBox::Show(gcnew System::String(L"Ошибка формирования отчёта: ") + ex->Message);
    }
}

void MainFormAdmin::ExportToCsv(DataTable^ dataTable, String^ filePath)
{
    StreamWriter^ writer = gcnew StreamWriter(filePath, false, System::Text::Encoding::UTF8);
    try
    {
        for each (DataColumn ^ column in dataTable->Columns)
        {
            writer->Write(L"\"" + column->ColumnName + L"\"");
            if (column->Ordinal < dataTable->Columns->Count - 1) writer->Write(L";");
        }
        writer->WriteLine();

        for each (DataRow ^ row in dataTable->Rows)
        {
            for each (DataColumn ^ column in dataTable->Columns)
            {
                String^ value = row[column]->ToString();
                value = value->Replace(L"\"", L"\"\"");
                writer->Write(L"\"" + value + L"\"");
                if (column->Ordinal < dataTable->Columns->Count - 1) writer->Write(L";");
            }
            writer->WriteLine();
        }
        MessageBox::Show(gcnew System::String(L"Отчёт экспортирован: ") + filePath, gcnew System::String(L"Успех"));
    }
    finally { writer->Close(); }
}

bool MainFormAdmin::VerifyAdminPassword()
{
    Form^ passwordForm = gcnew Form();
    passwordForm->Text = gcnew System::String(L"Подтверждение пароля");
    passwordForm->Size = System::Drawing::Size(380, 170);
    passwordForm->StartPosition = FormStartPosition::CenterParent;
    passwordForm->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;

    Label^ lblMessage = gcnew Label();
    lblMessage->Text = gcnew System::String(L"Введите пароль администратора:");
    lblMessage->SetBounds(20, 20, 250, 25);
    passwordForm->Controls->Add(lblMessage);

    TextBox^ txtPassword = gcnew TextBox();
    txtPassword->SetBounds(20, 50, 320, 23);
    txtPassword->PasswordChar = '*';
    passwordForm->Controls->Add(txtPassword);

    Button^ btnOk = gcnew Button();
    btnOk->Text = gcnew System::String(L"Подтвердить");
    btnOk->SetBounds(130, 90, 100, 30);
    btnOk->DialogResult = System::Windows::Forms::DialogResult::OK;
    passwordForm->Controls->Add(btnOk);

    if (passwordForm->ShowDialog() == System::Windows::Forms::DialogResult::OK)
    {
        return txtPassword->Text == adminPassword;
    }
    return false;
}

void MainFormAdmin::OnAddClick(Object^ sender, EventArgs^ args)
{
    if (!VerifyAdminPassword())
    {
        MessageBox::Show(gcnew System::String(L"Неверный пароль!"), gcnew System::String(L"Доступ запрещён"));
        return;
    }
    RegisterForm^ regForm = gcnew RegisterForm(sqlConnection);
    regForm->ShowDialog();
    LoadEmployees();
    LoadEmployeeCombo();
}

void MainFormAdmin::OnEditClick(Object^ sender, EventArgs^ args)
{
    if (dgvEmployees->SelectedRows->Count == 0) return;
    if (!VerifyAdminPassword())
    {
        MessageBox::Show(gcnew System::String(L"Неверный пароль!"), gcnew System::String(L"Доступ запрещён"));
        return;
    }
    int employeeId = Convert::ToInt32(dgvEmployees->SelectedRows[0]->Cells["Employee_ID"]->Value);
    RegisterForm^ editForm = gcnew RegisterForm(sqlConnection, employeeId);
    editForm->ShowDialog();
    LoadEmployees();
    LoadEmployeeCombo();
}

void MainFormAdmin::OnDeleteClick(Object^ sender, EventArgs^ args)
{
    if (dgvEmployees->SelectedRows->Count == 0) return;
    if (!VerifyAdminPassword())
    {
        MessageBox::Show(gcnew System::String(L"Неверный пароль!"), gcnew System::String(L"Доступ запрещён"));
        return;
    }

    System::Windows::Forms::DialogResult result = MessageBox::Show(
        gcnew System::String(L"Удалить сотрудника? Все отметки времени будут удалены!"),
        gcnew System::String(L"Подтверждение"), MessageBoxButtons::YesNo, MessageBoxIcon::Warning);

    if (result == System::Windows::Forms::DialogResult::Yes)
    {
        try
        {
            int employeeId = Convert::ToInt32(dgvEmployees->SelectedRows[0]->Cells["Employee_ID"]->Value);
            SqlCommand^ command = gcnew SqlCommand(L"DELETE FROM TimeEntry WHERE Employee_ID = @ID; DELETE FROM Employee WHERE Employee_ID = @ID", sqlConnection);
            command->Parameters->AddWithValue(L"@ID", employeeId);
            command->ExecuteNonQuery();
            MessageBox::Show(gcnew System::String(L"Сотрудник удалён!"), gcnew System::String(L"Успех"));
            LoadEmployees();
            LoadEmployeeCombo();
            LoadTimeEntries();
        }
        catch (Exception^ ex) { MessageBox::Show(gcnew System::String(L"Ошибка: ") + ex->Message); }
    }
}

void MainFormAdmin::OnRefreshClick(Object^ sender, EventArgs^ args)
{
    LoadEmployees();
    LoadEmployeeCombo();
}

void MainFormAdmin::OnEmployeesSelectionChanged(Object^ sender, EventArgs^ args)
{
    bool hasSelection = dgvEmployees->SelectedRows->Count > 0;
    btnEdit->Enabled = hasSelection;
    btnDelete->Enabled = hasSelection;
}

void MainFormAdmin::OnFilterClick(Object^ sender, EventArgs^ args)
{
    LoadTimeEntries();
}

void MainFormAdmin::OnCheckInClick(Object^ sender, EventArgs^ args)
{
    if (cmbEmployee->SelectedIndex == -1)
    {
        MessageBox::Show(gcnew System::String(L"Выберите сотрудника!"));
        return;
    }

    String^ selectedItem = cmbEmployee->SelectedItem->ToString();
    int employeeId = Int32::Parse(selectedItem->Substring(0, selectedItem->IndexOf(L" -")));

    try
    {
        SqlCommand^ command = gcnew SqlCommand(
            L"INSERT INTO TimeEntry (Employee_ID, Entry_Type, Entry_Time) VALUES (@EmployeeID, 'CheckIn', GETDATE())",
            sqlConnection);
        command->Parameters->AddWithValue(L"@EmployeeID", employeeId);
        command->ExecuteNonQuery();
        MessageBox::Show(gcnew System::String(L"Отметка о приходе добавлена!"));
        LoadTimeEntries();
    }
    catch (Exception^ ex) { MessageBox::Show(gcnew System::String(L"Ошибка: ") + ex->Message); }
}

void MainFormAdmin::OnCheckOutClick(Object^ sender, EventArgs^ args)
{
    if (cmbEmployee->SelectedIndex == -1)
    {
        MessageBox::Show(gcnew System::String(L"Выберите сотрудника!"));
        return;
    }

    String^ selectedItem = cmbEmployee->SelectedItem->ToString();
    int employeeId = Int32::Parse(selectedItem->Substring(0, selectedItem->IndexOf(L" -")));

    try
    {
        SqlCommand^ command = gcnew SqlCommand(
            L"INSERT INTO TimeEntry (Employee_ID, Entry_Type, Entry_Time) VALUES (@EmployeeID, 'CheckOut', GETDATE())",
            sqlConnection);
        command->Parameters->AddWithValue(L"@EmployeeID", employeeId);
        command->ExecuteNonQuery();
        MessageBox::Show(gcnew System::String(L"Отметка об уходе добавлена!"));
        LoadTimeEntries();
    }
    catch (Exception^ ex) { MessageBox::Show(gcnew System::String(L"Ошибка: ") + ex->Message); }
}

void MainFormAdmin::OnGenerateReportClick(Object^ sender, EventArgs^ args)
{
    LoadReport();
}

void MainFormAdmin::OnExportCsvClick(Object^ sender, EventArgs^ args)
{
    if (dgvReport->Rows->Count == 0)
    {
        MessageBox::Show(gcnew System::String(L"Нет данных для экспорта!"));
        return;
    }
    if (saveFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
    {
        try
        {
            DataTable^ dataTable = safe_cast<DataTable^>(dgvReport->DataSource);
            ExportToCsv(dataTable, saveFileDialog->FileName);
        }
        catch (Exception^ ex) { MessageBox::Show(gcnew System::String(L"Ошибка: ") + ex->Message); }
    }
}
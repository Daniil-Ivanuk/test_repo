#include "../include/RegisterForm.h"

RegisterForm::RegisterForm(SqlConnection^ connection)
{
    sqlConnection = connection;
    isEditMode = false;
    editingEmployeeId = -1;
    isRegistered = false;
    newEmployeeId = -1;
    InitializeComponent();
    LoadPositions();
}

RegisterForm::RegisterForm(SqlConnection^ connection, int employeeId)
{
    sqlConnection = connection;
    isEditMode = true;
    editingEmployeeId = employeeId;
    isRegistered = false;
    newEmployeeId = -1;
    InitializeComponent();
    LoadPositions();
    LoadEmployeeData();
}

void RegisterForm::InitializeComponent()
{
    if (isEditMode) this->Text = gcnew System::String(L"Редактирование сотрудника");
    else this->Text = gcnew System::String(L"Регистрация сотрудника");

    this->Size = System::Drawing::Size(480, 550);
    this->StartPosition = FormStartPosition::CenterParent;
    this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
    this->MaximizeBox = false;

    int yPos = 20, leftMargin = 20, labelWidth = 110, fieldWidth = 300;

    Label^ lblLastName = gcnew Label(); lblLastName->Text = gcnew System::String(L"Фамилия:*"); lblLastName->SetBounds(leftMargin, yPos, labelWidth, 25); this->Controls->Add(lblLastName);
    txtLastName = gcnew TextBox(); txtLastName->SetBounds(leftMargin + labelWidth, yPos, fieldWidth, 25); this->Controls->Add(txtLastName); yPos += 35;

    Label^ lblFirstName = gcnew Label(); lblFirstName->Text = gcnew System::String(L"Имя:*"); lblFirstName->SetBounds(leftMargin, yPos, labelWidth, 25); this->Controls->Add(lblFirstName);
    txtFirstName = gcnew TextBox(); txtFirstName->SetBounds(leftMargin + labelWidth, yPos, fieldWidth, 25); this->Controls->Add(txtFirstName); yPos += 35;

    Label^ lblMiddleName = gcnew Label(); lblMiddleName->Text = gcnew System::String(L"Отчество:"); lblMiddleName->SetBounds(leftMargin, yPos, labelWidth, 25); this->Controls->Add(lblMiddleName);
    txtMiddleName = gcnew TextBox(); txtMiddleName->SetBounds(leftMargin + labelWidth, yPos, fieldWidth, 25); this->Controls->Add(txtMiddleName); yPos += 35;

    Label^ lblPosition = gcnew Label(); lblPosition->Text = gcnew System::String(L"Должность:*"); lblPosition->SetBounds(leftMargin, yPos, labelWidth, 25); this->Controls->Add(lblPosition);
    cmbPosition = gcnew ComboBox(); cmbPosition->SetBounds(leftMargin + labelWidth, yPos, fieldWidth, 27); cmbPosition->DropDownStyle = ComboBoxStyle::DropDownList; this->Controls->Add(cmbPosition); yPos += 35;

    Label^ lblLogin = gcnew Label(); lblLogin->Text = gcnew System::String(L"Логин:*"); lblLogin->SetBounds(leftMargin, yPos, labelWidth, 25); this->Controls->Add(lblLogin);
    txtLogin = gcnew TextBox(); txtLogin->SetBounds(leftMargin + labelWidth, yPos, fieldWidth, 25); this->Controls->Add(txtLogin); yPos += 35;

    Label^ lblPassword = gcnew Label(); lblPassword->Text = gcnew System::String(L"Пароль:*"); lblPassword->SetBounds(leftMargin, yPos, labelWidth, 25); this->Controls->Add(lblPassword);
    txtPassword = gcnew TextBox(); txtPassword->SetBounds(leftMargin + labelWidth, yPos, fieldWidth, 25); txtPassword->PasswordChar = '*'; this->Controls->Add(txtPassword); yPos += 35;

    Label^ lblConfirm = gcnew Label(); lblConfirm->Text = gcnew System::String(L"Подтверждение:*"); lblConfirm->SetBounds(leftMargin, yPos, labelWidth, 25); this->Controls->Add(lblConfirm);
    txtConfirmPassword = gcnew TextBox(); txtConfirmPassword->SetBounds(leftMargin + labelWidth, yPos, fieldWidth, 25); txtConfirmPassword->PasswordChar = '*'; this->Controls->Add(txtConfirmPassword); yPos += 35;

    Label^ lblRole = gcnew Label(); lblRole->Text = gcnew System::String(L"Роль:*"); lblRole->SetBounds(leftMargin, yPos, labelWidth, 25); this->Controls->Add(lblRole);
    cmbRole = gcnew ComboBox(); cmbRole->SetBounds(leftMargin + labelWidth, yPos, fieldWidth, 27); cmbRole->DropDownStyle = ComboBoxStyle::DropDownList;
    cmbRole->Items->Add(L"Employee"); cmbRole->Items->Add(L"Admin"); cmbRole->SelectedIndex = 0; this->Controls->Add(cmbRole); yPos += 35;

    if (isEditMode)
    {
        chkIsActive = gcnew CheckBox(); chkIsActive->Text = gcnew System::String(L"Сотрудник активен");
        chkIsActive->SetBounds(leftMargin + labelWidth, yPos, 150, 25); chkIsActive->Checked = true; this->Controls->Add(chkIsActive); yPos += 40;
    }

    btnSave = gcnew Button(); btnSave->Text = isEditMode ? gcnew System::String(L"Сохранить") : gcnew System::String(L"Зарегистрировать");
    btnSave->SetBounds(leftMargin + 60, yPos, 120, 35); btnSave->Click += gcnew EventHandler(this, &RegisterForm::OnSaveClick); this->Controls->Add(btnSave);

    btnCancel = gcnew Button(); btnCancel->Text = gcnew System::String(L"Отмена");
    btnCancel->SetBounds(leftMargin + 200, yPos, 100, 35); btnCancel->Click += gcnew EventHandler(this, &RegisterForm::OnCancelClick); this->Controls->Add(btnCancel); yPos += 45;

    Label^ lblRequired = gcnew Label(); lblRequired->Text = gcnew System::String(L"* Поля, обязательные для заполнения");
    lblRequired->SetBounds(leftMargin, yPos, 250, 25); lblRequired->ForeColor = Color::Red; this->Controls->Add(lblRequired);
}

void RegisterForm::LoadPositions()
{
    try
    {
        SqlCommand^ command = gcnew SqlCommand(L"SELECT Position_ID, Position_Name FROM Position ORDER BY Position_Name", sqlConnection);
        SqlDataReader^ reader = command->ExecuteReader();
        cmbPosition->Items->Clear();
        while (reader->Read()) cmbPosition->Items->Add(reader["Position_Name"]->ToString());
        reader->Close();
        if (cmbPosition->Items->Count > 0) cmbPosition->SelectedIndex = 0;
    }
    catch (Exception^ ex) { MessageBox::Show(gcnew System::String(L"Ошибка загрузки должностей: ") + ex->Message); }
}

void RegisterForm::LoadEmployeeData()
{
    try
    {
        SqlCommand^ command = gcnew SqlCommand(
            L"SELECT e.Last_Name, e.First_Name, e.Middle_Name, p.Position_Name, "
            L"e.Login_Name, e.Role_Name, e.Is_Active "
            L"FROM Employee e INNER JOIN Position p ON e.Position_ID = p.Position_ID "
            L"WHERE e.Employee_ID = @EmployeeID", sqlConnection);
        command->Parameters->AddWithValue(L"@EmployeeID", editingEmployeeId);

        SqlDataReader^ reader = command->ExecuteReader();
        if (reader->Read())
        {
            txtLastName->Text = reader["Last_Name"]->ToString();
            txtFirstName->Text = reader["First_Name"]->ToString();
            txtMiddleName->Text = (reader["Middle_Name"] != DBNull::Value) ? reader["Middle_Name"]->ToString() : L"";
            cmbPosition->SelectedItem = reader["Position_Name"]->ToString();
            txtLogin->Text = reader["Login_Name"]->ToString();
            cmbRole->SelectedItem = reader["Role_Name"]->ToString();
            if (isEditMode) chkIsActive->Checked = Convert::ToBoolean(reader["Is_Active"]);

            txtPassword->Text = L"********";
            txtConfirmPassword->Text = L"********";
            txtPassword->Enabled = false;
            txtConfirmPassword->Enabled = false;
        }
        reader->Close();
    }
    catch (Exception^ ex) { MessageBox::Show(gcnew System::String(L"Ошибка загрузки: ") + ex->Message); }
}

bool RegisterForm::ValidateInputs()
{
    if (String::IsNullOrWhiteSpace(txtLastName->Text)) { MessageBox::Show(gcnew System::String(L"Введите фамилию!")); txtLastName->Focus(); return false; }
    if (String::IsNullOrWhiteSpace(txtFirstName->Text)) { MessageBox::Show(gcnew System::String(L"Введите имя!")); txtFirstName->Focus(); return false; }
    if (cmbPosition->SelectedIndex == -1) { MessageBox::Show(gcnew System::String(L"Выберите должность!")); cmbPosition->Focus(); return false; }
    if (String::IsNullOrWhiteSpace(txtLogin->Text)) { MessageBox::Show(gcnew System::String(L"Введите логин!")); txtLogin->Focus(); return false; }

    Regex^ loginRegex = gcnew Regex(L"^[a-zA-Z0-9_]+$");
    if (!loginRegex->IsMatch(txtLogin->Text)) { MessageBox::Show(gcnew System::String(L"Логин: только буквы, цифры, _")); txtLogin->Focus(); return false; }

    if (!isEditMode)
    {
        if (String::IsNullOrWhiteSpace(txtPassword->Text)) { MessageBox::Show(gcnew System::String(L"Введите пароль!")); txtPassword->Focus(); return false; }
        if (txtPassword->Text->Length < 4) { MessageBox::Show(gcnew System::String(L"Пароль минимум 4 символа!")); txtPassword->Focus(); return false; }
        if (txtPassword->Text != txtConfirmPassword->Text) { MessageBox::Show(gcnew System::String(L"Пароли не совпадают!")); txtConfirmPassword->Focus(); return false; }
    }
    return true;
}

void RegisterForm::OnSaveClick(Object^ sender, EventArgs^ args)
{
    if (!ValidateInputs()) return;

    try
    {
        if (isEditMode)
        {
            SqlCommand^ command = gcnew SqlCommand(
                L"UPDATE Employee SET Last_Name=@LN, First_Name=@FN, Middle_Name=@MN, "
                L"Position_ID=(SELECT Position_ID FROM Position WHERE Position_Name=@PN), "
                L"Login_Name=@Login, Role_Name=@Role, Is_Active=@Active WHERE Employee_ID=@ID", sqlConnection);
            command->Parameters->AddWithValue(L"@ID", editingEmployeeId);
            command->Parameters->AddWithValue(L"@LN", txtLastName->Text);
            command->Parameters->AddWithValue(L"@FN", txtFirstName->Text);
            command->Parameters->AddWithValue(L"@MN", String::IsNullOrWhiteSpace(txtMiddleName->Text) ? DBNull::Value : (Object^)txtMiddleName->Text);
            command->Parameters->AddWithValue(L"@PN", cmbPosition->SelectedItem->ToString());
            command->Parameters->AddWithValue(L"@Login", txtLogin->Text);
            command->Parameters->AddWithValue(L"@Role", cmbRole->SelectedItem->ToString());
            command->Parameters->AddWithValue(L"@Active", chkIsActive->Checked);
            command->ExecuteNonQuery();
            MessageBox::Show(gcnew System::String(L"Данные обновлены!"), gcnew System::String(L"Успех"));
            isRegistered = true;
            this->Close();
        }
        else
        {
            SqlCommand^ command = gcnew SqlCommand(
                L"INSERT INTO Employee (Last_Name, First_Name, Middle_Name, Position_ID, Login_Name, Password_, Role_Name, Is_Active) "
                L"SELECT @LN, @FN, @MN, Position_ID, @Login, @Pass, @Role, 1 FROM Position WHERE Position_Name=@PN; "
                L"SELECT SCOPE_IDENTITY()", sqlConnection);
            command->Parameters->AddWithValue(L"@LN", txtLastName->Text);
            command->Parameters->AddWithValue(L"@FN", txtFirstName->Text);
            command->Parameters->AddWithValue(L"@MN", String::IsNullOrWhiteSpace(txtMiddleName->Text) ? DBNull::Value : (Object^)txtMiddleName->Text);
            command->Parameters->AddWithValue(L"@PN", cmbPosition->SelectedItem->ToString());
            command->Parameters->AddWithValue(L"@Login", txtLogin->Text);
            command->Parameters->AddWithValue(L"@Pass", txtPassword->Text);
            command->Parameters->AddWithValue(L"@Role", cmbRole->SelectedItem->ToString());

            newEmployeeId = Convert::ToInt32(command->ExecuteScalar());
            MessageBox::Show(gcnew System::String(L"Сотрудник зарегистрирован! ID: ") + newEmployeeId, gcnew System::String(L"Успех"));
            isRegistered = true;
            this->Close();
        }
    }
    catch (Exception^ ex)
    {
        if (ex->Message->Contains(L"UNIQUE") || ex->Message->Contains(L"Duplicate"))
            MessageBox::Show(gcnew System::String(L"Логин уже существует!"), gcnew System::String(L"Ошибка"));
        else
            MessageBox::Show(gcnew System::String(L"Ошибка: ") + ex->Message, gcnew System::String(L"Ошибка"));
    }
}

void RegisterForm::OnCancelClick(Object^ sender, EventArgs^ args)
{
    this->Close();
}
#include "../include/Role.h"

RoleSelectionForm::RoleSelectionForm(SqlConnection^ connection)
{
    sqlConnection = connection;
    selectedRole = UserRole::None;
    InitializeComponent();
}

void RoleSelectionForm::InitializeComponent()
{
    this->Text = gcnew System::String(L"Выбор роли пользователя");
    this->Size = System::Drawing::Size(480, 420);
    this->StartPosition = FormStartPosition::CenterScreen;
    this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
    this->MaximizeBox = false;

    Label^ lblTitle = gcnew Label();
    lblTitle->Text = gcnew System::String(L"Система учёта рабочего времени");
    lblTitle->SetBounds(50, 20, 380, 35);
    lblTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 14, FontStyle::Bold);
    lblTitle->TextAlign = ContentAlignment::MiddleCenter;
    this->Controls->Add(lblTitle);

    btnAdmin = gcnew Button();
    btnAdmin->Text = gcnew System::String(L"Администратор");
    btnAdmin->SetBounds(60, 80, 160, 50);
    btnAdmin->Click += gcnew EventHandler(this, &RoleSelectionForm::OnAdminClick);
    this->Controls->Add(btnAdmin);

    btnEmployee = gcnew Button();
    btnEmployee->Text = gcnew System::String(L"Сотрудник");
    btnEmployee->SetBounds(250, 80, 160, 50);
    btnEmployee->Click += gcnew EventHandler(this, &RoleSelectionForm::OnEmployeeClick);
    this->Controls->Add(btnEmployee);

    grpLogin = gcnew GroupBox();
    grpLogin->Text = gcnew System::String(L"Вход в систему");
    grpLogin->SetBounds(60, 150, 350, 140);
    grpLogin->Visible = false;
    this->Controls->Add(grpLogin);

    Label^ lblLogin = gcnew Label();
    lblLogin->Text = gcnew System::String(L"Логин:");
    lblLogin->SetBounds(20, 30, 70, 25);
    grpLogin->Controls->Add(lblLogin);

    txtLogin = gcnew TextBox();
    txtLogin->SetBounds(100, 30, 220, 23);
    grpLogin->Controls->Add(txtLogin);

    Label^ lblPassword = gcnew Label();
    lblPassword->Text = gcnew System::String(L"Пароль:");
    lblPassword->SetBounds(20, 65, 70, 25);
    grpLogin->Controls->Add(lblPassword);

    txtPassword = gcnew TextBox();
    txtPassword->SetBounds(100, 65, 220, 23);
    txtPassword->PasswordChar = '*';
    grpLogin->Controls->Add(txtPassword);

    btnLogin = gcnew Button();
    btnLogin->Text = gcnew System::String(L"Войти");
    btnLogin->SetBounds(100, 100, 90, 28);
    btnLogin->Click += gcnew EventHandler(this, &RoleSelectionForm::OnLoginClick);
    grpLogin->Controls->Add(btnLogin);

    lblError = gcnew Label();
    lblError->SetBounds(200, 103, 130, 23);
    lblError->ForeColor = Color::Red;
    grpLogin->Controls->Add(lblError);

    btnExit = gcnew Button();
    btnExit->Text = gcnew System::String(L"Выход");
    btnExit->SetBounds(370, 330, 80, 30);
    btnExit->Click += gcnew EventHandler(this, &RoleSelectionForm::OnExitClick);
    this->Controls->Add(btnExit);
}

bool RoleSelectionForm::AuthenticateUser(
    String^ login,
    String^ password,
    int% employeeId,
    String^% fullName,
    String^% role)
{
    if (sqlConnection == nullptr || sqlConnection->State != ConnectionState::Open)
    {
        MessageBox::Show(gcnew System::String(L"Нет подключения к базе данных!"), gcnew System::String(L"Ошибка"));
        return false;
    }

    try
    {
        SqlCommand^ command = gcnew SqlCommand(
            L"SELECT Employee_ID, First_Name, Last_Name, Role_Name FROM Employee WHERE Login_Name = @LoginName AND Password_ = @Password AND Is_Active = 1",
            sqlConnection);
        command->Parameters->AddWithValue(L"@LoginName", login);
        command->Parameters->AddWithValue(L"@Password", password);

        SqlDataReader^ reader = command->ExecuteReader();

        if (reader->Read())
        {
            employeeId = reader->GetInt32(0);
            fullName = reader->GetString(1) + L" " + reader->GetString(2);
            role = reader->GetString(3);
            reader->Close();
            return true;
        }

        reader->Close();
        return false;
    }
    catch (Exception^ ex)
    {
        MessageBox::Show(gcnew System::String(L"Ошибка аутентификации: ") + ex->Message);
        return false;
    }
}

void RoleSelectionForm::OnAdminClick(Object^ sender, EventArgs^ args)
{
    selectedRole = UserRole::Admin;
    this->Close();
}

void RoleSelectionForm::OnEmployeeClick(Object^ sender, EventArgs^ args)
{
    grpLogin->Visible = true;
    grpLogin->BringToFront();
}

void RoleSelectionForm::OnLoginClick(Object^ sender, EventArgs^ args)
{
    if (String::IsNullOrWhiteSpace(txtLogin->Text))
    {
        lblError->Text = gcnew System::String(L"Введите логин!");
        txtLogin->Focus();
        return;
    }

    if (String::IsNullOrWhiteSpace(txtPassword->Text))
    {
        lblError->Text = gcnew System::String(L"Введите пароль!");
        txtPassword->Focus();
        return;
    }

    int employeeId;
    String^ fullName;
    String^ role;

    if (AuthenticateUser(txtLogin->Text, txtPassword->Text, employeeId, fullName, role))
    {
        selectedRole = UserRole::Employee;
        this->Close();
    }
    else
    {
        lblError->Text = gcnew System::String(L"Неверный логин или пароль!");
        txtPassword->Clear();
        txtPassword->Focus();
    }
}

void RoleSelectionForm::OnExitClick(Object^ sender, EventArgs^ args)
{
    selectedRole = UserRole::None;
    this->Close();
}
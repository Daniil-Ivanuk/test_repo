#pragma once
#include "../include/all_includes.h"

public enum class UserRole
{
    None,
    Admin,
    Employee
};

public ref class RoleSelectionForm : public Form
{
private:
    Button^ btnAdmin;
    Button^ btnEmployee;
    Button^ btnExit;
    GroupBox^ grpLogin;
    TextBox^ txtLogin;
    TextBox^ txtPassword;
    Button^ btnLogin;
    Label^ lblError;

    SqlConnection^ sqlConnection;
    UserRole selectedRole;

    void InitializeComponent();
    bool AuthenticateUser(String^ login, String^ password, int% employeeId, String^% fullName, String^% role);
    void OnAdminClick(Object^ sender, EventArgs^ args);
    void OnEmployeeClick(Object^ sender, EventArgs^ args);
    void OnLoginClick(Object^ sender, EventArgs^ args);
    void OnExitClick(Object^ sender, EventArgs^ args);

public:
    RoleSelectionForm(SqlConnection^ connection);

    property UserRole SelectedRole
    {
        UserRole get() { return selectedRole; }
    }
};
#pragma once
#include "../include/all_includes.h"

public ref class RegisterForm : public Form
{
private:
    SqlConnection^ sqlConnection;
    bool isEditMode;
    int editingEmployeeId;
    bool isRegistered;
    int newEmployeeId;

    TextBox^ txtLastName;
    TextBox^ txtFirstName;
    TextBox^ txtMiddleName;
    ComboBox^ cmbPosition;
    TextBox^ txtLogin;
    TextBox^ txtPassword;
    TextBox^ txtConfirmPassword;
    ComboBox^ cmbRole;
    CheckBox^ chkIsActive;

    Button^ btnSave;
    Button^ btnCancel;

    void InitializeComponent();
    void LoadPositions();
    void LoadEmployeeData();
    bool ValidateInputs();
    void OnSaveClick(Object^ sender, EventArgs^ args);
    void OnCancelClick(Object^ sender, EventArgs^ args);

public:
    RegisterForm(SqlConnection^ connection);
    RegisterForm(SqlConnection^ connection, int employeeId);

    property bool IsRegistered { bool get() { return isRegistered; } }
    property int NewEmployeeId { int get() { return newEmployeeId; } }
};
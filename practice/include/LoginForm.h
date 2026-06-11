#pragma once
#include "../include/all_includes.h"

public ref class LoginForm : public Form
{
private:
    TextBox^ serverTextBox;
    TextBox^ databaseTextBox;
    TextBox^ usernameTextBox;
    TextBox^ passwordTextBox;
    CheckBox^ integratedAuthCheckBox;
    Button^ connectButton;
    Button^ registerButton;

    SqlConnection^ sqlConnection;
    bool is_connected;

    void InitializeComponent();
    void integratedAuthCheckBox_CheckedChanged(Object^ sender, EventArgs^ args);
    void connectButton_Click(Object^ sender, EventArgs^ args);
    void registerButton_Click(Object^ sender, EventArgs^ args);

public:
    LoginForm();

    property bool IsConnected
    {
        bool get() { return is_connected; }
    }

    property SqlConnection^ Connection
    {
        SqlConnection^ get() { return sqlConnection; }
    }
};
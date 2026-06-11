#include "../include/Main.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    try
    {
        LoginForm^ loginForm = gcnew LoginForm();
        Application::Run(loginForm);

        if (!loginForm->IsConnected)
        {
            return 0;
        }

        RoleSelectionForm^ roleForm = gcnew RoleSelectionForm(loginForm->Connection);
        Application::Run(roleForm);

        if (roleForm->SelectedRole == UserRole::None)
        {
            return 0;
        }

        if (roleForm->SelectedRole == UserRole::Admin)
        {
            MainFormAdmin^ adminForm = gcnew MainFormAdmin(loginForm->Connection);
            Application::Run(adminForm);
        }
        else if (roleForm->SelectedRole == UserRole::Employee)
        {
            MessageBox::Show(gcnew System::String(L"Форма для сотрудника будет доступна в следующей версии."));
        }
    }
    catch (Exception^ ex)
    {
        MessageBox::Show(gcnew System::String(L"Критическая ошибка: ") + ex->Message);
    }

    return 0;
}
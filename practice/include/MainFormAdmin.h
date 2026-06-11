#pragma once
#include "../include/all_includes.h"
#include "../include/RegisterForm.h"

public ref class MainFormAdmin : public Form
{
private:
    SqlConnection^ sqlConnection;
    String^ adminPassword;

    TabControl^ tabMain;

    TabPage^ tabEmployees;
    DataGridView^ dgvEmployees;
    Button^ btnAdd;
    Button^ btnEdit;
    Button^ btnDelete;
    Button^ btnRefresh;

    TabPage^ tabTimeEntries;
    ComboBox^ cmbEmployee;
    DateTimePicker^ dtpStartDate;
    DateTimePicker^ dtpEndDate;
    Button^ btnFilter;
    Button^ btnCheckIn;
    Button^ btnCheckOut;
    DataGridView^ dgvTimeEntries;
    Label^ lblLastEntry;

    TabPage^ tabReports;
    DateTimePicker^ dtpReportStart;
    DateTimePicker^ dtpReportEnd;
    Button^ btnGenerateReport;
    Button^ btnExportCsv;
    DataGridView^ dgvReport;
    SaveFileDialog^ saveFileDialog;

    void InitializeComponent();
    void LoadEmployees();
    void LoadEmployeeCombo();
    void LoadTimeEntries();
    void UpdateLastEntryInfo();
    void LoadReport();
    void ExportToCsv(DataTable^ dataTable, String^ filePath);
    bool VerifyAdminPassword();

    void OnAddClick(Object^ sender, EventArgs^ args);
    void OnEditClick(Object^ sender, EventArgs^ args);
    void OnDeleteClick(Object^ sender, EventArgs^ args);
    void OnRefreshClick(Object^ sender, EventArgs^ args);
    void OnEmployeesSelectionChanged(Object^ sender, EventArgs^ args);
    void OnFilterClick(Object^ sender, EventArgs^ args);
    void OnCheckInClick(Object^ sender, EventArgs^ args);
    void OnCheckOutClick(Object^ sender, EventArgs^ args);
    void OnGenerateReportClick(Object^ sender, EventArgs^ args);
    void OnExportCsvClick(Object^ sender, EventArgs^ args);

public:
    MainFormAdmin(SqlConnection^ connection);
};
CREATE DATABASE TimeTracking;
GO

USE TimeTracking;
GO

--Справочник должностей
CREATE TABLE Position (
    Position_ID INT PRIMARY KEY IDENTITY(1,1),
    Position_Name NVARCHAR(100) NOT NULL UNIQUE
);
GO

--Таблица сотрудников 
CREATE TABLE Employee (
    Employee_ID INT PRIMARY KEY IDENTITY(1,1),

    Last_Name NVARCHAR(50) NOT NULL,
    First_Name NVARCHAR(50) NOT NULL,
    Middle_Name NVARCHAR(50) NULL,
    
    --Организационная структура 
    Position_ID INT NOT NULL,
    
    --Учётные данные для входа
    Login_Name NVARCHAR(50) NOT NULL UNIQUE,
    Password_ NVARCHAR(255) NOT NULL, 
    
    --Роль 
    Role_Name NVARCHAR(20) NOT NULL CHECK (Role_Name IN ('Admin', 'Employee')),
    
    --Метаданные
    Hire_Date DATE NOT NULL DEFAULT CAST(GETDATE() AS DATE), --Дата приёма на работу (по умолчанию текущая)
    Is_Active BIT NOT NULL DEFAULT 1, --Работает сотрудник или нет
    
    -- Внешние ключи
    CONSTRAINT FK_Employee_Position FOREIGN KEY (Position_ID) REFERENCES Position(Position_ID)
    
);
GO

--Таблица отметок времени 
CREATE TABLE TimeEntry (
    TimeEntry_ID BIGINT PRIMARY KEY IDENTITY(1,1),
    
    Employee_ID INT NOT NULL,
    Entry_Type NVARCHAR(10) NOT NULL CHECK (Entry_Type IN ('CheckIn', 'CheckOut')),
    Entry_Time DATETIME NOT NULL DEFAULT GETDATE(),
    
    CONSTRAINT FK_TimeEntry_Employee FOREIGN KEY (Employee_ID) REFERENCES Employee(Employee_ID),
    
    -- Индекс для быстрого поиска по сотруднику и дате
    INDEX IX_TimeEntry_Employee_Time (Employee_ID, Entry_Time)
);
GO


INSERT INTO Position (Position_Name) VALUES 
(N'Председатель'),
(N'Заместитель председателя'),
(N'Помощник председателя'),
(N'Начальник отдела'),
(N'Заместитель начальник отдела'),
(N'Судья'),
(N'Помощник судьи'),
(N'Администратор приложения'),
(N'Секретарь зала заседаний'),
(N'Администратор суда');
GO

INSERT INTO Employee (Last_Name, First_Name, Position_ID, Login_Name, Password_, Role_Name, Is_Active)
SELECT N'Admin', N'System', Position_ID, 'admin', 'admin123', 'Admin', 1
FROM Position WHERE Position_Name = N'Администратор приложения';
GO


-- Включаем учётную запись sa
ALTER LOGIN sa ENABLE;
GO

-- Устанавливаем пароль (придумайте свой)
ALTER LOGIN sa WITH PASSWORD = 'Password_F0R_DB', CHECK_POLICY = OFF;
GO

EXEC xp_instance_regread N'HKEY_LOCAL_MACHINE', N'Software\Microsoft\MSSQLServer\MSSQLServer', N'LoginMode';
GO

-- Включаем учётную запись sa
ALTER LOGIN sa ENABLE;
GO

-- Сбрасываем пароль (временно используем простой)
ALTER LOGIN sa WITH PASSWORD = N'123456', CHECK_POLICY = OFF;
GO

-- Проверяем статус
SELECT name, is_disabled FROM sys.sql_logins WHERE name = 'sa';
GO
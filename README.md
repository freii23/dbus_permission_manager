# dbus_permission_manager

Для того, чтобы все заработало, необходимо в Qt сперва запустить permission_service и time_service и уже потом test_service - тест это как раз приложение для тестирования сервисов времени и разрешения.
Чтобы все заработало также необходимо поднять на локальном ПК БД SQLITE - для этого достаточно в консоли sqlite создать БДшку в месте с путем, аналогичным моему:  /home/sandor/prog/test_case_omp/test_case_omp_db.db.

Также необходимо создать таблицу logs, вот ее код:
CREATE TABLE logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT, -- Автоинкрементное поле
    filepath NVARCHAR(120),
    perm_code INTEGER
);

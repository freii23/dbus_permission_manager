# dbus_permission_manager

ТЗ:
1. Реализовать DBus сервис с именем com.system.permissions на сессионной шине с использованием языка C++, который имеет 2
метода: <br /> <br />
void RequestPermission(permissionEnumCode: int) <br />
Который получает путь до исполняемого файла, вызвавшего данный метод, по dbus имени клиента (пример, как это можно
сделать) и сохраняет в базу данных SQLite информацию о том, что данный исполняемый файла запросил заданное
разрешение. В случае ошибки метод должен возвращать DBus ошибку с человекочитаемым сообщением. <br /> <br />
bool CheckApplicationHasPermission(applicationExecPath: String, permissionEnumCode: int) <br />
Который проверяет имеется ли у приложение с заданным путем до исполняемого файла заданное разрешение. В случае
ошибки метод должен возвращать DBus ошибку с человекочитаемым сообщением. <br /> <br />
Реализация: permissions_service

2. Реализовать DBus сервис com.system.time на сессионной шине с использованием языка C++, который имеет 1 метод: <br /> <br />
uint64 GetSystemTime() <br />
Который возврашает timestamp текущего системного времени. Однако перед этим, он получает путь до исполняемого файла,
вызвавшего данный метод, по dbus имени клиента (пример, как это можно сделать) и проверяет при помощи DBus сервиса com.
system.permissions, имеет ли данный исполняемый файл разрешение SystemTime. В случае если исполняемый файл не
имеет разрешения SystemTime, должна возвращаться ошибка UnauthorizedAccess с человекочитаемым сообщением. В
случае любой другой ошибки должна возвращаться обычная ошибка с человекочитаемым сообщением. <br /> <br />
Реализация: time_service

3. Реализовать приложение с использованием языка C++, которое: <br /> <br />
Пробует запросить у сервиса com.system.time текущее время и в случае ошибки UnauthorizedAccess запрашивает
разрешение SystemTime у сервиса com.system.permissions, после чего пытается повторить запрос текущего времени. <br /> <br />
После получения timestamp'a текущего времени, выводит его на экран в человекочитемом виде. <br /> <br />
Реализация: test_drive

Для того, чтобы все заработало, необходимо в Qt сперва запустить permission_service и time_service и уже потом test_service - тест это как раз приложение для тестирования сервисов времени и разрешения.
Чтобы все заработало также необходимо поднять на локальном ПК БД SQLITE - для этого достаточно в консоли sqlite создать БДшку в месте с путем, аналогичным моему:  /home/sandor/prog/test_case_omp/test_case_omp_db.db.

Также необходимо создать таблицу logs, вот ее код: <br />
CREATE TABLE logs ( <br />
    id INTEGER PRIMARY KEY AUTOINCREMENT, <br /> 
    filepath NVARCHAR(120), <br />
    perm_code INTEGER <br />
); <br />

по ТЗ требовалось всего 2 метода в permission service, но я добавил ещё один, который узнает путь до исполняемого файла. Также не совсем понятно, как с заданным набором параметров в функции RequestPermission можно получить этот самый путь (по идее, должен быть еще один параметр, принимающий dbus имя сервиса, вызывающего наш - в моем случае это com.system.test, в ТЗ это был com.example.example - 3 часть ТЗ), поэтому пока что захардкодил.

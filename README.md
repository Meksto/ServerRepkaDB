# ServerRepkaDB

## Описание

Данный серверный код представляет собой TCP/IP приложение, которое создает сокет и ожидает подключения от клиентов. Сервер также взаимодействует с базой данных IRepka, которая представляет из себя простой словарь, где значения хранятся по ключам. Сервер принимает сообщения от клиентов и с помощью функции command_responsive() распознает команды и вызывает соответствующие функции для их обработки.

## Запуск сервера
Для запуска сервера необходимо указать желаемый порт и максимальное количество пользователей, которые могут одновременно подключиться к серверу. Пример запуска сервера:

`./server_repka <port> <max_users>`

Где `<port>` - номер порта, на котором сервер будет ожидать подключения, и
`<max_users>` - максимальное количество пользователей, которые могут одновременно подключиться к серверу.



## Ограничения

Ключи в базе данных могут содержать только следующие символы: `0-9, a-z, A-Z`

Если максимальное количество пользователей, указанное при запуске сервера, достигнуто, и еще один клиент попытается подключиться, сервер отправит сообщение об ошибке клиенту и закроет соединение.

## Перечень команд, которые обрабатывает сервер от клиента

    set <key> <value> - создает новую запись в базе данных с указанным ключом и значением.
    get <key> - возвращает значение из базы данных по указанному ключу, если запись существует, иначе возвращает сообщение "Doesn't exist".
    del <key> - удаляет запись из базы данных по указанному ключу, если запись существует, иначе возвращает сообщение "Doesn't exist".
    count - возвращает количество записей в базе данных.
    save_dump <filename> - сохраняет базу данных в указанный файл.
    load_dump <filename> - загружает данные из указанного файла в базу данных, предварительно очищая ее от существующих записей.
    
    Ввод любой другой команды будет возвращать сообщение "Incorrect input".

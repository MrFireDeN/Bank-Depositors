# Банковское приложение для управления вкладами

## Описание проекта

Банковское приложение для управления вкладами представляет собой клиент-серверное приложение, разработанное на языке программирования C++ с использованием фреймворка Qt. Проект включает в себя две основные части: клиентскую и серверную.

### Функционал клиентской части:
- Создание новых вкладов.
- Просмотр списка существующих вкладов.
- Обновление информации о вкладах.
- Удаление вкладов.

### Функционал серверной части:
- Запуск сервера для обработки запросов клиентов.
- Загрузка и сохранение базы данных вкладов.
- Обработка запросов клиентов на добавление, удаление, обновление, просмотр и получение количества вкладов.

### Клиентская часть:
- Создание новых вкладов осуществляется путем заполнения формы с данными клиента, такими как ФИО, номер счета, сумма вклада и т.д. После заполнения данных и нажатия кнопки "Добавить" информация отправляется на сервер для обработки.
- Просмотр списка существующих вкладов позволяет клиенту просматривать все вклады, хранящиеся в базе данных на сервере. Список вкладов обновляется при запуске приложения и после каждого добавления, удаления или обновления записи.
- Обновление информации о вкладах происходит при выборе вклада из списка и нажатии кнопки "Сохранить". Клиенту предоставляется форма для внесения изменений в выбранный вклад, после чего измененная информация отправляется на сервер.
- Удаление вкладов осуществляется при выборе вклада из списка и нажатии кнопки "Удалить". Выбранный вклад удаляется из базы данных на сервере.

### Серверная часть:
- Запуск сервера осуществляется при запуске приложения. Сервер ожидает подключения клиентов и обрабатывает их запросы.
- Загрузка и сохранение базы данных вкладов происходит при запуске и завершении работы сервера. Данные о вкладах хранятся в файле `main.dd` на сервере.
- Обработка запросов клиентов на добавление, удаление, обновление, просмотр и получение количества вкладов осуществляется в соответствующих методах сервера. Каждый запрос от клиента обрабатывается поочередно в цикле, пока не будет получен запрос на завершение работы сервера.

### Технологии:
- Язык программирования: C++
- Фреймворк: Qt
- Среда разработки: Qt Creator

## Инструкции по запуску:
1. Запустить сервер, выполнив файл `bankserver.exe`.
2. Запустить клиентское приложение, выполнив файл `bankclient.exe`.
3. Использовать функционал клиентского приложения для управления вкладами.

# [ПЗ-3] Символьный драйвер в старом стиле


### Задание
1) разработать модуль ядра с параметром
2) запустить поток ядра, который счастотой заданной параметром выводит в log сообщения, переданные параметром

### Описание кода
#### Реализация операции чтения
```device_read```: Реализует чтение данных из устройства.

```*offset```: Смещение, с которого начинается чтение.

```copy_to_user```: Копирует данные из буфера ядра (device_buffer) в пользовательский буфер (user_buffer).
#### Реализация операции записи
```device_write```: Реализует запись данных в устройство.

```copy_from_user```: Копирует данные из пользовательского буфера (user_buffer) в буфер ядра (device_buffer).
#### Реализация управления устройством через ioctl
```device_ioctl```: Обрабатывает управляющие команды.

```IOCTL_CLEAR_BUFFER```: Очищает буфер устройства с помощью memset.

```IOCTL_RESIZE_BUFFER```: 
Изменяет размер буфера:

Используется krealloc для перераспределения памяти.
Если выделение памяти не удалось, возвращается ошибка -ENOMEM.
Новый буфер инициализируется нулями.



```file_operations```: Структура, которая связывает функции чтения, записи и управления с устройством.
### Результаты
#### В обычном режиме
Average = 0.5282460
Max = 1.0912608

![image](https://github.com/user-attachments/assets/c2a6a451-1528-4f7a-ae9b-a0d01d1718b9)

#### C высоким приоритетом
Average = 0.5360
Max = 1.09187
![image](https://github.com/user-attachments/assets/b84fc715-5af8-4175-8565-f8b95a2f4366)

#### C низким приоритетом
Average = 0.5431
Max = 1.1127
![image](https://github.com/user-attachments/assets/edb1b254-db39-43a6-a3ec-2089ab483679)

#### SCHED_FIFO
Average = 0.5236
Max = 1.1675
![image](https://github.com/user-attachments/assets/0c2d8d71-3598-4947-9d92-317dac530d0a)

#### Нагрузка HDD
Average = 0.5578212
Max = 1.11209
![image](https://github.com/user-attachments/assets/19d059ad-527b-43d9-ae8c-c2524070827e)

#### Нагрузка сети
Average = 0.53907
Max = 1.154608
![image](https://github.com/user-attachments/assets/bbb01391-efb4-4f26-97b6-afeee1b18c64)


---
title: 'Korni3 cli params doc'
draft: false
category: 
	- 'Корни'
	- 'Корни-версия-3'

tags:
	- 'личный суверенитет'
	- Корни3
	- korni3
	- документация
creation_date: 2022-02-12T22:39:38.296910+00:00
last_modified_timestamp: 2022-02-12T22:39:38.296910+00:00
commentable: true
editable: true
---

# Korni3

> Технология личного информационного суверенитета.

1. `korni3` это файловая система хранения данных, управления данными с SQL возможностями. 
   1. ваши данные всегда есть как минимум у Вас, и доступ к ним вам никто не может запретить, перекрыть итд. 

2. `korni3` работает как быстрый "дописываемый журнал", сделанный так, чтобы его частями удобно было обмениваться (а ля KAfka)
3. `korni3` проверяет и создает "Цифровые Подписи", которые есть у каждой записи, ясно кто и когда её создал и изменил. 
4. Ретроспектива: `korni3` может хранить историю всех изменений. 
5. `korni3` это утилита командной строки. 
6. `korni3` можно использовать как более быструю нативную (с++) замену для python утилиты `sqlite-utils` с некоторыми улучшениями. 
7. `korni3` работает как замена kafka-sql , но не требует разворачивания серверов , их доступности итд, `korni3` работает с файлами и больше поход на `git`  при использовании. 
8. `korni3` идеально подойдет как основа хранилища данных  децентрализованных сервисов , как система управления научными данными,  тем , кто парсит и обрабатывает данные, анализирует... и хочет добиться порядка в своих данных. 
9. `korni3` это "транспорт агностик" система - в ней нет сетевых функций и не должно их быть. Это Unix way файловый способ взаимодействия с системой.  Пользователя `korni3` юридически невозможно обвинить в "распространении чего-либо" т.к. в ней нет никаких транспортных функций. 
10. `korni3` сделан так, чтобы быть много-пользовательским хранилищем для децентрализованных систем , при этом не имея транспорта - это сеть , без сети. 
11. для `korni3` существует 100500 множество "сторонних" способов обмена данными. Перекрыть их все совершенно. невозможно. С `korni3` можно работать даже без интернета - через флешки, через радио, через mesh сети, факс, "лапшу", модемы и конечно десятки способов с разной идеологией через белый и тёмный интернет...
12. `korni3` использует "Цифровые подписи", но может использовать опционально и любое шифрование, внутри обучающих материалов будет для Вас курс по крипто-безопасности. 

> Короче! я грамотно совместил :
>
> 1) идею `kafka storage`, выкинув, громоздкое, и добавив `gzip` сжатие И 
> 2) концепт `kafka-sql` (из журнала в индексы, доступные через sql, пишем в журнал, читаем sql)  И 
> 3) файловую `slite3` субд базу + удобные расширения для ГЕО индексов И 
> 4) цифровые подписи для проверки подлинности (в кафка такого не было)
> 5) `ipfs` + `torrent` + `sha`  ... кодирование файлов для хранения, для дедубликации и удобства обмена файлами
>
> чтобы не зависеть от сетей и протоколов - это реализуется отдельными системами. 

**Планы**

> переделать интернет, так, чтобы пользователь там полноправным хозяином всех своих данных. 

1. для `korni3`  создается такой состав графических приложений: 
   1. Интернет магазин / "маркетплейс", типа Avito, Kufar, Яндекс.Маркет, и многих подобных... Там будет гораздо больше категорий, в том числе спрос. 
   2. статьи с обсуждением, как , например, на Я.Дзене
   3. Огромная база знаний, с медиа материалами (как @youtube) и с текстами, резервные копии ценных сайтов итд. 
   4. Мощная система управления знаниями (спрашивайте)  и обучения. 
   5. система управления предприятием, ERP, MES, PDM, планирование, склад, ... всё нужные модули. 



## SYNOPSYS

```bash
# genkey <nickname> [<bits; 4096 by default>]
$ korni3 genkey demo

# print to stdout keys nicknames
$ korni3 keys
testUser
demo

# korni3 login <nickname>
$ korni3 login demo
demo	MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAvCEIchAnnO5hIdiuFt/+op8uBbRdh67lPjkLU6Mj00Q+DoFuOWBb3pRfLXdd6xEg+VUa7liBunrm99a5og/DYp98p6+Lh68yCSzLzDm9bPkvVuRsjVjm4Fngc6/sKK+dC04KtnDDoQ5J+rIAHaX2WrgyTwu1Hc+M0ENeAazSXwCNJMc+Fgutj0ofaobRlelvx/FndZKQUFWkbVBf6LY08kSut7sPadBrKMbf1Oyg1k5FUTSbgOBvUmyIx+qOfINCv/TsJxLCSzyCZXRJFJ40EGz1bO2XbJRf8EIWwNYk6VnKZ70dJqClBBAWxVM1w7k5t8aDyBTsEic4uLZLyIN5t9ou9klWW467pTUwhkHJbD2xG9bB/YFLOQY9bQxOx6CRbw4ZegLhk+M3AjG3P20/ouv7psYz9o08pfgpvlkaICGklvWnCBAB4AC1FI/e83j6JVqrFTn+ZJCVxED5OaiKqioaK2bWu7QHgo/7UgQZpKwB7qRadzg1mQ7k1Vd0wy8iBnQajc1akH4RiK/IwMqsI+Z9JlP4vWLHWPdUQzPzghKsTj2HbBBzBhvrrMV5szYuxQj/vVYtEcNPapigWinhwdoZCFNGfXHDeKt/iph1GYqOD0krFz96K2glphFfa74CkaOjTXUnn2f4lI/Pz82GwnqFG1/8W0LXKO4RcgxZYvUCAwEAAQ==

# create|insert <dbName> <tableName> [<jsonRecord>]
# create|insert <dbName> <tableName> # json read from stdin
$ korni3 insert test testTable '{"id": 1, "name":"korni demo tutorial"}'

$ korni3 rows test testTable --csv --table  -c id -c zT -c name --where 'id = 1'
  id          zT  name
----  ----------  ---------------------
   1  1664036153  korni demo tutorial
   1  1664036310  korni 1 demo tutorial
   1  1664036336  korni 4 demo tutorial

$ korni3 repack

# korni3 file <dbName> <fileOrDirPath>
$ korni3 file test /home/mp/Qt/QtCreator-v8.0.1/bin/qt.conf
GFS-Qme6DcxwC2SABnttae5ndAPzNyk4G2KHmxFas7NqFHPFhy-T-bd3541672cd9a9c83d7ac8c6f74a7f6fe272f734-M-8a5ff4aeb511b1cfae11787da6e28a99-S256-7b3a3f93a63341651d7ef129ddca235f33cf061e434cd866e3d1695cb4cee402	"/home/mp/Qt/QtCreator-v8.0.1/bin/qt.conf"
{"id":"GFS-Qme6DcxwC2SABnttae5ndAPzNyk4G2KHmxFas7NqFHPFhy-T-bd3541672cd9a9c83d7ac8c6f74a7f6fe272f734-M-8a5ff4aeb511b1cfae11787da6e28a99-S256-7b3a3f93a63341651d7ef129ddca235f33cf061e434cd866e3d1695cb4cee402","path":"\/home\/mp\/Qt\/QtCreator-v8.0.1\/bin\/qt.conf","size":"84","creationTime":"Thu Aug 11 12:10:22 2022\n","lastModTime":"Thu Aug 11 12:10:22 2022\n","fileOut":"\/home\/mp\/Qt\/QtCreator-v8.0.1\/bin\/qt.conf: ASCII text\n"}

$ korni3 file test /home/mp/Qt/QtCreator-v8.0.1/bin/qt.conf 2>/dev/null 
{"id":"GFS-Qme6DcxwC2SABnttae5ndAPzNyk4G2KHmxFas7NqFHPFhy-T-bd3541672cd9a9c83d7ac8c6f74a7f6fe272f734-M-8a5ff4aeb511b1cfae11787da6e28a99-S256-7b3a3f93a63341651d7ef129ddca235f33cf061e434cd866e3d1695cb4cee402","path":"\/home\/mp\/Qt\/QtCreator-v8.0.1\/bin\/qt.conf","size":"84","creationTime":"Thu Aug 11 12:10:22 2022\n","lastModTime":"Thu Aug 11 12:10:22 2022\n","fileOut":"\/home\/mp\/Qt\/QtCreator-v8.0.1\/bin\/qt.conf: ASCII text\n"}

$ korni3 insert-files images *.gif
.....

# setgeo <lat like 58.05139>,<lat like 38.83802>
# setgeo <lon like 58.05139> <lon like 38.83802>
# INFO : set current user current/new geoplace.
$ korni3 58.05139 38.83802

# просканировать журнал, чтобы обновить БД
# фильтрация изменений по дате, гео позиции, пользователям  ...
# scan <containerName> [<filter option> см. documentation]
$ korni3 scan test # [options]

# db <containerName>
$ sqlite3  `korni3 db test` '.tables' 
files      node       testTable
```



## TUTORIAL 

### genkey ; generate keypairs  ; Генерируем пары ключей с человеко-читаемым прозвищем .  

> **Содержимое папки `/home/mp/.config/korni/MY-KEYS/` никому нельзя показывать!** Лучше хранить это на личной "флешке", а путь пробрасывать через символьную ссылку. 

```sh
# genkey <nickname> [<bits; 4096 by default>]
$ korni3 genkey demo
$ ls ~/.config/korni/MY-KEYS/demo*
/home/mp/.config/korni/MY-KEYS/demo-pub.pem  /home/mp/.config/korni/MY-KEYS/demo-secret.pem
```

### keys ; Просмотр списка пар ключей

read "special directory" with keys
print to stdout available keys (basicaly key nicknames) in format // **TODO**  публичные ключи

```sh
# print to stdout keys nicknames
$ korni3 keys
testUser
demo
```

### login ;  выбрать рабочий ключ

если успех: Печатает в stdout  tsv строку из двух полей псевдоним и идентификатор в виде публичного ключа. Этот длинный публичный ключ и есть внутренний идентификатор пользователя. 

если ошибка: печатает в stderr сообщение ошибки

```sh
# korni3 login <nickname>
# select surrent nickname "mask" of user; And crypto key
# if success print: <usernick> <userid=publickkey>
$ korni3 login demo
demo	MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAvCEIchAnnO5hIdiuFt/+op8uBbRdh67lPjkLU6Mj00Q+DoFuOWBb3pRfLXdd6xEg+VUa7liBunrm99a5og/DYp98p6+Lh68yCSzLzDm9bPkvVuRsjVjm4Fngc6/sKK+dC04KtnDDoQ5J+rIAHaX2WrgyTwu1Hc+M0ENeAazSXwCNJMc+Fgutj0ofaobRlelvx/FndZKQUFWkbVBf6LY08kSut7sPadBrKMbf1Oyg1k5FUTSbgOBvUmyIx+qOfINCv/TsJxLCSzyCZXRJFJ40EGz1bO2XbJRf8EIWwNYk6VnKZ70dJqClBBAWxVM1w7k5t8aDyBTsEic4uLZLyIN5t9ou9klWW467pTUwhkHJbD2xG9bB/YFLOQY9bQxOx6CRbw4ZegLhk+M3AjG3P20/ouv7psYz9o08pfgpvlkaICGklvWnCBAB4AC1FI/e83j6JVqrFTn+ZJCVxED5OaiKqioaK2bWu7QHgo/7UgQZpKwB7qRadzg1mQ7k1Vd0wy8iBnQajc1akH4RiK/IwMqsI+Z9JlP4vWLHWPdUQzPzghKsTj2HbBBzBhvrrMV5szYuxQj/vVYtEcNPapigWinhwdoZCFNGfXHDeKt/iph1GYqOD0krFz96K2glphFfa74CkaOjTXUnn2f4lI/Pz82GwnqFG1/8W0LXKO4RcgxZYvUCAwEAAQ==

# select not exist key - simulate error
$ korni3 login dfg
ERROR: with this nickname keypair not exists 

# for print current user just dont use nickname parameter
$ korni3 login
demo	MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAvCEIchAnnO5hIdiuFt/+op8uBbRdh67lPjkLU6Mj00Q+DoFuOWBb3pRfLXdd6xEg+VUa7liBunrm99a5og/DYp98p6+Lh68yCSzLzDm9bPkvVuRsjVjm4Fngc6/sKK+dC04KtnDDoQ5J+rIAHaX2WrgyTwu1Hc+M0ENeAazSXwCNJMc+Fgutj0ofaobRlelvx/FndZKQUFWkbVBf6LY08kSut7sPadBrKMbf1Oyg1k5FUTSbgOBvUmyIx+qOfINCv/TsJxLCSzyCZXRJFJ40EGz1bO2XbJRf8EIWwNYk6VnKZ70dJqClBBAWxVM1w7k5t8aDyBTsEic4uLZLyIN5t9ou9klWW467pTUwhkHJbD2xG9bB/YFLOQY9bQxOx6CRbw4ZegLhk+M3AjG3P20/ouv7psYz9o08pfgpvlkaICGklvWnCBAB4AC1FI/e83j6JVqrFTn+ZJCVxED5OaiKqioaK2bWu7QHgo/7UgQZpKwB7qRadzg1mQ7k1Vd0wy8iBnQajc1akH4RiK/IwMqsI+Z9JlP4vWLHWPdUQzPzghKsTj2HbBBzBhvrrMV5szYuxQj/vVYtEcNPapigWinhwdoZCFNGfXHDeKt/iph1GYqOD0krFz96K2glphFfa74CkaOjTXUnn2f4lI/Pz82GwnqFG1/8W0LXKO4RcgxZYvUCAwEAAQ==
```

### insert/create   ;   добавление данных 

Опции: 

* `--skip-db-update` создаются только подписанные записи на диске, но не добавляются в sql базу данных. Это удобно при быстром  пакетном добавлении данных, или когда записи поступают на `STDIN ` чтобы быстро их рпосто добавить и потом уже сделать `korni3 scan <db>`...

```sh
# create|insert <dbName> <tableName> [<jsonRecord>]  [--skip-db-update]
# create|insert <dbName> <tableName> # json read from stdin

# вставим запись 
$ korni3 insert test testTable '{"id": 1, "name":"korni demo tutorial"}'
$ echo $?
0

# вставим записи, которые приходят из консоли
$ korni3 insert test testTable 
> {"id": 1, "name":"korni 1 demo tutorial"}
> {"id": 1, "name":"korni 2 demo tutorial", "f2": false}
> {"id": 1, "name":"korni 3 demo tutorial", "f2": true, "f3":"f3 value"}
> {"id": 1, "name":"korni 4 demo tutorial"}
$ echo $?
0
```

В результате получим данные в БД (поле `zU` условно не показано), 

```sh
$ korni3 rows test testTable --csv --table  -c id -c zT -c name --where 'id = 1'
  id          zT  name
----  ----------  ---------------------
   1  1664036153  korni demo tutorial
   1  1664036310  korni 1 demo tutorial
   1  1664036336  korni 4 demo tutorial
```

или графически

![image-20220924191957634](/home/mp/.config/Typora/typora-user-images/image-20220924191957634.png)

Первичным ключом является пара (`id`, `zT`), где `zT` - epoch UTC timestamp,  т.е. момент времени когда создана данныя запись. 

В записи, так же, всегда присутствует публичный ключ (pubkey) пользователя, который создал запись. 

Прикладное приложение может выбирать для каждого id  объекта только одну версию с наиболее поздним моментом времени создания. 

То, что данные автоматически появились в таблице - это результат опtрации scan автоматической операции scan, которая вызвалась автоматичеcки при вставке записей в "журнал". 

На самом деле главная функция операции insert - создание записей в журнале. 

- `/home/mp/.config/korni/` - файлы системы korni3
- `/home/mp/.config/korni/test` - данные контейнера с именем test (это имя фигурирует в командах выше)

посмотрим файлы записей таблицы tastTable, которые были созданы командой insert:

```sh
$ find ~/.config/korni/test -type f | grep T--testTable--
/home/mp/.config/korni/test/tables/2022/2022-9-24/U-646/60e/e5431584f0fd0cf0f86b7fa04ff23bf3d0a554c5930786ffd7155cd29c7c/T--testTable--D-2022-9-24-GH-uf7d5zbvz7qt.jsonnl.gz

$ find ~/.config/korni/test -type f | grep T--testTable-- | xargs ls -l 
-rw-rw-r-- 1 mp mp 6150 сен 24 19:18 /home/mp/.config/korni/test/tables/2022/2022-9-24/U-646/60e/e5431584f0fd0cf0f86b7fa04ff23bf3d0a554c5930786ffd7155cd29c7c/T--testTable--D-2022-9-24-GH-uf7d5zbvz7qt.jsonnl.gz

$ find ~/.config/korni/test -type f | grep T--testTable-- | xargs file  
/home/mp/.config/korni/test/tables/2022/2022-9-24/U-646/60e/e5431584f0fd0cf0f86b7fa04ff23bf3d0a554c5930786ffd7155cd29c7c/T--testTable--D-2022-9-24-GH-uf7d5zbvz7qt.jsonnl.gz: gzip compressed data, max compression, from Unix, original size modulo 2^32 1538
```

Итак,  имеем файл с причудливым названием `T--testTable--D-2022-9-24-GH-uf7d5zbvz7qt.jsonnl.gz`   в папке с причудливым названием `tables/2022/2022-9-24/U-646/60e/e5431584f0fd0cf0f86b7fa04ff23bf3d0a554c5930786ffd7155cd29c7c/`

* Каждое изменение данных, продуцирует **новые записи** в этом и подобных файлах,  
* записи добавляются автоматически предсказуемо в нужные файлы. 
* система дописывает файлы "сегодняшнего дня", файлы предыдущих дат не трогает. 
* файлы сжаты gzip алгоритмом, внутри частичные подписанные ЦП записи в формате jsonnl, которые еще соответствуют определенным требованиям (созданы разными пользователями)
* каждый файл дописывается только одним пользователем , разные пользователи пишут "дополнения к таблицам файлы с разными именами

Имя самого файла содержит : 

* `T--testTable--` имя таблицы, "дополнения" к которой, содержатся в файле
* `D-2022-9-24-` - дата создания (UTC) дополнений в этом файле 
* `GH-uf7d5zbvz7qt`  - geoHash ; гео позиция точки , к которой относятся записи, которую пользователь мог указать, если желает.  (чтобы сразу по листингу быстрее получать обновления только интересующего региона, например для приложений с объявлениями)

Имя папки содержит: 

Год, дату, `sha256(user public key)` , разбитый на три части (три подпапки) по три значащих символа в первых двух частях. 

Это важно , чтобы файловая система быстро могла работать с миллиардами пользователей, и при этом можно было бы продолжать получать обновления инкрементально, начиная с нужной даты, и интересоваться обновлениями по приоритетным пользователям (подписки, каналы, друзья, команда ... ). 

Ещё это важно, чтобы файлы не пересекались конфликтами по пользователям, и можно было бы безболезненно мержить папки разных пользователей, объединяя их наборы  изменений. 



Пример содержимого файлов обновлений таблиц (распакуем только две записи):  

```sh
$ zcat /home/mp/.config/korni/test/tables/2022/2022-9-24/U-646/60e/e5431584f0fd0cf0f86b7fa04ff23bf3d0a554c5930786ffd7155cd29c7c/T--testTable--D-2022-9-24-GH-uf7d5zbvz7qt.jsonnl.gz | head -n 2
{"id":"1","name":"korni demo tutorial","zT":"1664036153","zU":"MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAvCEIchAnnO5hIdiuFt\/+op8uBbRdh67lPjkLU6Mj00Q+DoFuOWBb3pRfLXdd6xEg+VUa7liBunrm99a5og\/DYp98p6+Lh68yCSzLzDm9bPkvVuRsjVjm4Fngc6\/sKK+dC04KtnDDoQ5J+rIAHaX2WrgyTwu1Hc+M0ENeAazSXwCNJMc+Fgutj0ofaobRlelvx\/FndZKQUFWkbVBf6LY08kSut7sPadBrKMbf1Oyg1k5FUTSbgOBvUmyIx+qOfINCv\/TsJxLCSzyCZXRJFJ40EGz1bO2XbJRf8EIWwNYk6VnKZ70dJqClBBAWxVM1w7k5t8aDyBTsEic4uLZLyIN5t9ou9klWW467pTUwhkHJbD2xG9bB\/YFLOQY9bQxOx6CRbw4ZegLhk+M3AjG3P20\/ouv7psYz9o08pfgpvlkaICGklvWnCBAB4AC1FI\/e83j6JVqrFTn+ZJCVxED5OaiKqioaK2bWu7QHgo\/7UgQZpKwB7qRadzg1mQ7k1Vd0wy8iBnQajc1akH4RiK\/IwMqsI+Z9JlP4vWLHWPdUQzPzghKsTj2HbBBzBhvrrMV5szYuxQj\/vVYtEcNPapigWinhwdoZCFNGfXHDeKt\/iph1GYqOD0krFz96K2glphFfa74CkaOjTXUnn2f4lI\/Pz82GwnqFG1\/8W0LXKO4RcgxZYvUCAwEAAQ==","zS":"ulLlynBK4i8oQF48\/66eKz0pQM+qqmOprU7eiGlBDEqTf3CRcl1NEStdGn1djEFN\n4t0WxS\/J\/3yQ2a8rFh7GbWoIAAdu4tIai2s3\/p4O0FXnEzl5rKlhFileCkbhPAME\nQzp9SrMzAmv1V1fSo5TYRTFM42or2PP2TtRdXbI8Ao+8Cq36ahlbyB5RKsv99kb6\n3gu8tUUl8OwxHE5rIx4ELLq74xLcJJxn1ipR5e156Y1PWeDEVFWK9dcfX8u1VKS4\n\/4OdRgwJsoiVEI\/2pIvUJAmoFdE8\/IoejnsBpGyc2zmFu0iqEFtsN+NWFKB0oTqJ\ndr1FMWamwpLOncuWhXcDIx1MzENCHzwRMOlVY6P5xSDfSk2WUA0rN3mMrNYgAFjL\nZDTP\/f9VFdwuE3w2TX6xw7hX5C4UTCRDQWW7BZeGWOKmMNIYDF0hok+gM9RB7HQm\npbHxH3YZPnLO64FzrN2FNKgPgg+WVXMH8IHfc4qnptmq3NKy6AufVMlqnwDKgYjB\nrI\/ttpvaNRPRr36nnHhEr5w+A3SlHZtqgimWHyNeP7bMbiTfz59D3tnYX5gQGcT3\n8lvHtZSpbm5WiiDZV300U1ecfL+pI66vefAPgJGB5mj\/+CtxiDh+QC7YWxg7veK5\n5B1nGuYvqN75Nx\/l6x8nCx+S4e9ncJQBJt75xuZVIEQ="}
{"id":"1","name":"korni 1 demo tutorial","zT":"1664036310","zU":"MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAvCEIchAnnO5hIdiuFt\/+op8uBbRdh67lPjkLU6Mj00Q+DoFuOWBb3pRfLXdd6xEg+VUa7liBunrm99a5og\/DYp98p6+Lh68yCSzLzDm9bPkvVuRsjVjm4Fngc6\/sKK+dC04KtnDDoQ5J+rIAHaX2WrgyTwu1Hc+M0ENeAazSXwCNJMc+Fgutj0ofaobRlelvx\/FndZKQUFWkbVBf6LY08kSut7sPadBrKMbf1Oyg1k5FUTSbgOBvUmyIx+qOfINCv\/TsJxLCSzyCZXRJFJ40EGz1bO2XbJRf8EIWwNYk6VnKZ70dJqClBBAWxVM1w7k5t8aDyBTsEic4uLZLyIN5t9ou9klWW467pTUwhkHJbD2xG9bB\/YFLOQY9bQxOx6CRbw4ZegLhk+M3AjG3P20\/ouv7psYz9o08pfgpvlkaICGklvWnCBAB4AC1FI\/e83j6JVqrFTn+ZJCVxED5OaiKqioaK2bWu7QHgo\/7UgQZpKwB7qRadzg1mQ7k1Vd0wy8iBnQajc1akH4RiK\/IwMqsI+Z9JlP4vWLHWPdUQzPzghKsTj2HbBBzBhvrrMV5szYuxQj\/vVYtEcNPapigWinhwdoZCFNGfXHDeKt\/iph1GYqOD0krFz96K2glphFfa74CkaOjTXUnn2f4lI\/Pz82GwnqFG1\/8W0LXKO4RcgxZYvUCAwEAAQ==","zS":"hWkCUCSNemMQ3t4hf\/27Y0ZnnRbQqIONmfsCvoyOKbzGL1iBNyzqo6EsuwJu6iZ8\nXFlmPyvDX5NMMxs02XFo6R+NKBTI\/AfupFbWLOEe8vUQC6K3+1IxGOihNwCOOcz+\nl3jtfJaw4sWt1aXUjvboENHh4ty8M1ddTPb8TwEHppqS\/\/81XZqEaSNOx0qoHl7M\n6pj\/ITjDBTw5ZgOgp91VtPXgDgchuDWPlhSax8viq\/Sh51BZDdtqzMVg\/7WNudsM\nIVWioI16+rSGLeyf5TodRhBLXcE0B78uNWaIF5qAs2BbcfovFf\/R37SVQDZuEAJF\nXIGJ32IMeZb06yXe2+5uSK3\/GMs2kmy3Mj1\/8Uj4IYfW9Fwg6ohCiYc45gpKd9pr\nmjRUAX1IhHcNmek7Z8y4GbxZDXQ+tQOsnxKI8i8UQwShWs2SoISKLj2BwCy2Xe2h\nr9wAdcqwH9qG5Ch94KYZRpTBq9K5+LLZvPtekvLp2u5Y101ooHNydQAGY5s0RscL\nSDLCU5Ti29RH94GlvO1JU5uzR+kCqyXRRWdU8\/+skbgwd+vZmCO+qec7+wkOEWvP\nvmkFpD2ZRXQp49Nk5ZY+KsmuW0DNnIoX4\/04\/6\/NEV9hRHWwh33YfMZZZQjzdlZ3\nIZq79pf5s3vhvy5rVtLjX2x+ieg2lFn+s5v+OwKBLgA="}
```

Здесь записи громоздкие. 

В каждой записи есть одинаковое поле `zU` (пользователь), которое очень хорошо жмётся gzip и не влияет на размер. 

В каждой записи есть поле `zS` (цифровая подпись) она есть только в сырых записях, после проверки это поле не попадает в базу данных. 

Записи не обязательно формируются вами, из может формировать стороннее ПО, и при подписывании могут быть ошибки... "Битые записи" могут лежать в журналах, но **ни одна запись с не правильной подписью не попадает в базу данных из журнала**. 

Этот выход можно легко обработать например `jq` утилитой. Но у нас есть вариант получше : `korni3 verify` например. 



### repack ; пересжатие

При формировании файлов , для большей скорости, используется "дополнений для таблиц" используется "дописывание gzip блоков в файл", а уже вчерашние и более старые файлы не нужно дописывать, для них можно применить наилучшее сжатие с предварительным вычислением общего словаря. 

```sh
$ korni3 repack
```

эта команда сжимает все таблицы заново

Пример вывода команды (ratio %, old size, new size, filename;  в конце экономия на  диске)

```bash
mp@mp-strela:/media/mp/ac22fd5c-3daf-4f5a-a4bf-65b346984f64/BACKUP/KORNI-DATA/yt-korni-WfoAvg6ooB$ korni3 repack yt comments
41.93	32053	13440	'/home/mp/.config/korni/yt/tables/2022/2022-10-2/U-2de/ed2/214e94c4a3bf858f87a8714837e810615afac70b4affb0b783a14e7b26f2/T--files--D-2022-10-2-GH-uf7d5zbvz7qt.jsonnl.gz"
70.29	2757	1938	'/home/mp/.config/korni/yt/tables/2022/2022-10-2/U-2de/ed2/214e94c4a3bf858f87a8714837e810615afac70b4affb0b783a14e7b26f2/T--thumbinal--D-2022-10-2-GH-uf7d5zbvz7qt.jsonnl.gz"
54.39	9573	5207	'/home/mp/.config/korni/yt/tables/2022/2022-10-2/U-2de/ed2/214e94c4a3bf858f87a8714837e810615afac70b4affb0b783a14e7b26f2/T--subtitlesFiles--D-2022-10-2-GH-uf7d5zbvz7qt.jsonnl.gz"
64.37	3643	2345	'/home/mp/.config/korni/yt/tables/2022/2022-10-2/U-2de/ed2/214e94c4a3bf858f87a8714837e810615afac70b4affb0b783a14e7b26f2/T--playlists--D-2022-10-2-GH-uf7d5zbvz7qt.jsonnl.gz"
100.00	1243	1243	'/home/mp/.config/korni/yt/tables/2022/2022-10-1/U-2de/ed2/214e94c4a3bf858f87a8714837e810615afac70b4affb0b783a14e7b26f2/T--playlists--D-2022-10-1-GH-uf7d5zbvz7qt.jsonnl.gz"
50.32	366349	184332	'/home/mp/.config/korni/yt/tables/2022/2022-10-4/U-2de/ed2/214e94c4a3bf858f87a8714837e810615afac70b4affb0b783a14e7b26f2/T--videoFiles--D-2022-10-4-GH-uf7d5zbvz7qt.jsonnl.gz"
48.55	307271	149175	'/home/mp/.config/korni/yt/tables/2022/2022-10-4/U-2de/ed2/214e94c4a3bf858f87a8714837e810615afac70b4affb0b783a14e7b26f2/T--subtitlesFiles--D-2022-10-4-GH-uf7d5zbvz7qt.jsonnl.gz"
45.61	1336970	609757	'/home/mp/.config/korni/yt/tables/2022/2022-10-4/U-2de/ed2/214e94c4a3bf858f87a8714837e810615afac70b4affb0b783a14e7b26f2/T--comments--D-2022-10-4-GH-uf7d5zbvz7qt.jsonnl.gz"
49.96	422943	211295	'/home/mp/.config/korni/yt/tables/2022/2022-10-4/U-2de/ed2/214e94c4a3bf858f87a8714837e810615afac70b4affb0b783a14e7b26f2/T--files--D-2022-10-4-GH-uf7d5zbvz7qt.jsonnl.gz"
1304070
```



### file ; сохранение файлов 

#### главный способ

* Файлы не правильно хранить в базе данных 
  * хотя, в ряде случаев это разумно и я позже покажу как это можно удобно делать в korni3
* Кроме того, удобнее и правильнее делать, используя ХЕШ по контенту. 

> Korni3 transport agnistic ! - это значит, что `korni3` не имеет и не будет иметь сетевых функций, однако предоставляет все удобства для всех систем обмена файлами. 

Пример добавления файлов в систему `korni3`:  (добавлять можно и один файл и папку рекурсивно)

```sh
# korni3 file <dbName> <fileOrDirPath> [--skipmeta] [--ignore] [--rm] [--empty]
$ korni3 file test /home/mp/Qt/QtCreator-v8.0.1/bin/qt.conf
GFS-Qme6DcxwC2SABnttae5ndAPzNyk4G2KHmxFas7NqFHPFhy-T-bd3541672cd9a9c83d7ac8c6f74a7f6fe272f734-M-8a5ff4aeb511b1cfae11787da6e28a99-S256-7b3a3f93a63341651d7ef129ddca235f33cf061e434cd866e3d1695cb4cee402	"/home/mp/Qt/QtCreator-v8.0.1/bin/qt.conf"
{"id":"GFS-Qme6DcxwC2SABnttae5ndAPzNyk4G2KHmxFas7NqFHPFhy-T-bd3541672cd9a9c83d7ac8c6f74a7f6fe272f734-M-8a5ff4aeb511b1cfae11787da6e28a99-S256-7b3a3f93a63341651d7ef129ddca235f33cf061e434cd866e3d1695cb4cee402","path":"\/home\/mp\/Qt\/QtCreator-v8.0.1\/bin\/qt.conf","size":"84","creationTime":"Thu Aug 11 12:10:22 2022\n","lastModTime":"Thu Aug 11 12:10:22 2022\n","fileOut":"\/home\/mp\/Qt\/QtCreator-v8.0.1\/bin\/qt.conf: ASCII text\n"}

$ korni3 file test /home/mp/Qt/QtCreator-v8.0.1/bin/qt.conf 2>/dev/null 
{"id":"GFS-Qme6DcxwC2SABnttae5ndAPzNyk4G2KHmxFas7NqFHPFhy-T-bd3541672cd9a9c83d7ac8c6f74a7f6fe272f734-M-8a5ff4aeb511b1cfae11787da6e28a99-S256-7b3a3f93a63341651d7ef129ddca235f33cf061e434cd866e3d1695cb4cee402","path":"\/home\/mp\/Qt\/QtCreator-v8.0.1\/bin\/qt.conf","size":"84","creationTime":"Thu Aug 11 12:10:22 2022\n","lastModTime":"Thu Aug 11 12:10:22 2022\n","fileOut":"\/home\/mp\/Qt\/QtCreator-v8.0.1\/bin\/qt.conf: ASCII text\n"}
```

Ключи не обязательны, порядок любой :

* `--skipmeta`  - добавить сам файл в контейнер, но не писать в таблицу `files` метаинформацию
* `--skip-db-update` метаинформацию писать на диск, но бд не обновлять для скорости обработки (можно будет сделать обновить "пачкой" позже `korni3 scan` )
* `--ignore` - проверить id  файла и если он есть в контейнере - не добавлять ни его ни метаинформацию.
* `--rm` после успешной обработки файлов - исходные файлы удалить с диска. *Используется для экономии места на диске - произойдет перенос файлов в контейнер, это выгодно для скриптов, которые скачивают, чтобы добавить, больших файлов видео и фото, устранения дубликатов...*
* `--empty` - после успешной обработки исходный файл усекается по размеру до нуля. Это выгодно для скриптов, которые скачивают контент для добавления, чтобы они знали с какого места продолжить работу в случае разрыва связи, и не скачивали уже скачанные файлы (они по наличию на диске часто проверяют)

Утилита печатает в STDOUT  и в STDERR каналы в разных форматах, чтобы удобнее было использоать korni3 в bash конвейерах. 

* в stdout - json (nl new line ended)  в котором 
  * korni3 file id  ,  о нём подробнее ниже
  * путь к файлу 
  * размер в байтах 
  * даты создания и последней модификации 
  * вывод диагностики test утилитой file  - информация о типе файле и его полях. 
  * в будущем список тестов будет дополнятся. 
* в stderr - только korni3 file id (состоит из хешей) И путь 

1. Сам файл добавляется в систему (так что с исходного места его уже можно удалить для экономии места)
2. в контейнер в таблицу  "files" добавляется запись о нём.  (тот самый json record)

вот сам файл : 

```sh
$ find ~/.config/korni/test -type f | grep Qme6DcxwC2SABnttae5ndAPzNyk4G2KHmxFas7NqFHPFhy
/home/mp/.config/korni/test/files/e6/Dc/xw/GFS-Qme6DcxwC2SABnttae5ndAPzNyk4G2KHmxFas7NqFHPFhy-T-bd3541672cd9a9c83d7ac8c6f74a7f6fe272f734-M-8a5ff4aeb511b1cfae11787da6e28a99-S256-7b3a3f93a63341651d7ef129ddca235f33cf061e434cd866e3d1695cb4cee402
```

структура пути файлов

* korni/**test**/files - место для файлов в контейнере test
* `e6/Dc/xw/GFS-Qme6DcxwC2S...`    Qm**e6**<u>Dc</u>**xw**C2S    разбивка файлов по папкам , имена папок образуются как части ipfs идентификатора файла,  ipfs по умолчанию для хешей по умолчанию использует  base58 кодирование, т.е. максимальное число папок в каждой папке на каждом уровне не превышает  58^2=3364  при таком числе папок в папке файловая система  обычно не тормозит , а вот при большем числе  символов это уже 58^3=195_112 уже многовато и ФС будет тормозить при  "ручной навигации",  на трёх уровнях это 3364^3=38_068_692_544  файлов если в каждой конечной папке будет только по одному файлу, что более  чем достаточно для одного контейнера, (и ничего не мешает в будущем заложить больше уровней.)

сам идентификатор файла (его имя в папке korni3) содержит такие хеши, в качестве частей

* `GFS-Qme6DcxwC2SABnttae5ndAPzNyk4G2KHmxFas7NqFHPFhy-` ipfs хеш по умолчанию, это всё таки "global file system"
* `T-bd3541672cd9a9c83d7ac8c6f74a7f6fe272f734-` torrent hash
* `M-8a5ff4aeb511b1cfae11787da6e28a99-` md5
* `S256-7b3a3f93a63341651d7ef129ddca235f33cf061e434cd866e3d1695cb4cee402`  sha256

*`korni3` дополняет лог `/tmp/TORRENT-error.log/`  выводом программы transmission-create (это возможные ошибки создания торрент файлов)*

Сторонние программы, которые будут управлять содержимым легко смогут по этим хешам ориентироваться. 
Вы, например,  можете смело добавить вашу папку files в torrent раздачу  и по hash находить файлы для скачивания (подобные "синхронизаторы" я предложу в рамках другого проекта). 

#### другой способ

Добавление base64 файла в "дополнения таблиц" - в сами таблицы попадает в итоге BLOB поле. 

```sh
$ korni3 insert-files images *.gif
..... 
```

По умолчанию схема полей будет такой 

```sql
CREATE TABLE [images] (
	[id] TEXT PRIMARY KEY, // korni3 file id
	[zT] LONG PRIMARY KEY, // record creation time
    [path] TEXT,
    [content] BLOB,
    [size] INTEGER,
    [zU] TEXT
);
```

Если добавить файл снова - будет другая запись! т.к. `path` - НЕ  PRIMARY KEY. Будьте аккуратны. 

Можно добавить опциями команды , например `-c path -c md5 -c mtime` ...

Доступные опции :

```
name
The name of the file, e.g. cleo.jpg

path
The path to the file relative to the root folder, e.g. pictures/cleo.jpg

fullpath
The fully resolved path to the image, e.g. /home/simonw/pictures/cleo.jpg

sha256
The SHA256 hash of the file contents

md5
The MD5 hash of the file contents

mode
The permission bits of the file, as an integer - you may want to convert this to octal

content
The binary file contents, which will be stored as a BLOB

content_text
The text file contents, which will be stored as TEXT

mtime
The modification time of the file, as floating point seconds since the Unix epoch

ctime
The creation time of the file, as floating point seconds since the Unix epoch

mtime_int
The modification time as an integer rather than a float

ctime_int
The creation time as an integer rather than a float

mtime_iso
The modification time as an ISO timestamp, e.g. 2020-07-27T04:24:06.654246

ctime_iso
The creation time is an ISO timestamp

size
The integer size of the file in bytes

stem
The filename without the extension - for file.txt.gz this would be file.txt

suffix
The file extension - for file.txt.gz this would be .gz
```



### setgeo ; указать местоположение пользователя 

* Команда получает координаты пользователя, кодирует их в формате geohash и запоминает. 
* не имеет отношение к контейнеру, а только к пользователю. 
* может быть использовано приложениями которым нужна гео функциональность из коробки
* помогает просто получать обновления данных из "своего региона"

```bash
# setgeo <lat like 58.05139>,<lat like 38.83802>
# setgeo <lon like 58.05139> <lon like 38.83802>
# INFO : set current user current/new geoplace.
$ korni3 58.05139 38.83802
```

### verify ; проверить записи ; helper utility ; служебная утилита

1. Команда **читает** jsonnl записи **с STDOUT**
2. проверяет подпись
3. если ок - удаляет поле подпись из записи и выводит запись на STDOUT
4. если ошибка - печатает их на STDERR

```sh
# korni3 verify
# INFO: verify record from changelist files
# record readed from stdin ; successfuly verified records go to stdout ; error print to stderr
$ korni3 verify
```

### scan ; обновить БД sqlite3 из проверенных по подписи "дополнений" в журналах. 

1. Команда получает ограничения (filter  - подстрока коорая должна рисутствовать в именах файлов на диске (смотри кодирование структуры файлов)), иначе обрабатывается всё 
2. по ним проверяет дополнения (change set)  
3. импортирует их  в БД. 

на псевдокоде работу команды можно выразить так: 

```sh
# pseudocode replace : 
korniFilterChangeSet(<user conditions>) -> paths | <split by tables; set table=...> | xargs zcat | korni verify | sqlite-utils <db> import <table> --pk=id --pk=zT --nl
```

Фильтровать изменения можно 

* **по дате** начиная с которой до текущего момента изменения применяются (всё, если не указана)
* **по конкретному списку аккаунтов**, или по всем, если не указано
* **по гео локации**, **с указанием радиуса от гео точки**, или по всей планете, если не указано иное

// function 
//write table updates to /sqlite/ db
// and filter files changes
// if <userList> are empty: handl all available users , else: only from list (updates from friends?)
// if <fromDate> are empty: handle all dates, else: new records from that date in past (format: 2001-09-11)
// if <geoHashPoint> are empty: handle all geo positions, else: only filechanges in <geoRadius > from point <geoHashPoint>
// before handle: verify sign in records
// for hadnle import use the external tool: `pip3 install sqlite-utils` OR 'pip3 install spyql' // TODO native

```sh

```

### db ; путь к базе для прямой работы

Утилита печатает на std out путь к файлу БД для прямой работы с базой

Надо для запросов чтения  через `sqlite3`
Используется например так:

```sh
 # db <containerName>
 # $ sqlite3 `korni3 db test` 'select * from ttt limit 5;'
 $ korni3 db test 
/home/mp/.config/korni/test/database.db

$ sqlite3 `korni3 db test` 'select * from ttt limit 5;'
Error: no such table: ttt

$ sqlite3  `korni3 db test` '.tables' 
files      node       testTable

$ sqlite3  `korni3 db test` "select path from files where fileOut like '%PNG%' limit 3;"
/doc/qtcreator/qtcreator-dev/images/qtcreator-tooltip-long.png
/doc/qtcreator/qtcreator-dev/images/firstplugin-pluginsetup.png
/doc/qtcreator/qtcreator-dev/images/qtcreator-menu-item.png

$ sqlite3  `korni3 db test` "select distinct  fileOut from files  limit 3;"
/home/mp/Qt/QtCreator-v8.0.1/bin/qt.conf: ASCII text

/home/mp/Qt/QtCreator-v8.0.1/share/doc/qtcreator/qtcreator-dev/extensionsystem-plugindetailsview.html: HTML document, ASCII text, with very long lines

/home/mp/Qt/QtCreator-v8.0.1/share/doc/qtcreator/qtcreator-dev/qtcreator-dev-wizards.html: HTML document, ASCII text, with very long lines

$ (echo .headers on ; echo 'select * from files  limit 2;') | sqlite3 -csv  `korni3 db test` | spyql 'select id, path, size, zT from csv to orjson'

{"id":"GFS-Qme6DcxwC2SABnttae5ndAPzNyk4G2KHmxFas7NqFHPFhy-T-bd3541672cd9a9c83d7ac8c6f74a7f6fe272f734-M-8a5ff4aeb511b1cfae11787da6e28a99-S256-7b3a3f93a63341651d7ef129ddca235f33cf061e434cd866e3d1695cb4cee402","path":"/home/mp/Qt/QtCreator-v8.0.1/bin/qt.conf","size":84,"zT":1663822473}
{"id":"GFS-Qmbm8kkiPR2MTW9if3ijReph8t77tvPzFQoFhM1dVDyTEX-T-451e2b13fffebbfedb0ed1036f51600e1a59a3dc-M-e0ad1c51fe2f8c17dfa254c1d1a59ba4-S256-f3a6e56801a8533c6987f523a7adfab0bb07f4ca30af1cf510a039b675671f3a","path":"/doc/qtcreator/qtcreator-dev/extensionsystem-plugindetailsview.html","size":4811,"zT":1663822565}
```

Для обработки (фильтрации, преобразования итд) данных тут есть все возможности. 


TODO 
* [ ] переделать epoch из секунд в точнее , поле zT

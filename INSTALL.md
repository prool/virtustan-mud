Prool MUD (Virtustan MUD)
http://mud.kharkov.org

**Как запустить свой мад сервер**

1. Свой мад сервер нужен не всем. Может, вы просто хотите поиграть в мад?
Виртустан мад доступен в онлайне по протоколу telnet:

telnet mud.kharkov.org 3000

Или любым мад-клиентом, например JMC, надо войти по адресу

#con mud.kharkov.org 3000

Есть и другие мады, их список можно найти здесь - http://www.mudconnector.su/MudList
или здесь http://muds.kharkov.org/muds.html

2. Если все таки вам нужен свой мад сервер, то Виртустан мад это один из лучших выборов,
так как у большинства других мадов (не у всех) код и файлы игровых зон закрыты,
а я распространяю код свободно.

3. Далее всё зависит от того, какая у вас операционная система. Для Виндовс проще всего
скачать и запустить уже сделанную мной откомпилированную сборку, которую можно взять
здесь http://files.mud.kharkov.org/vmud-windows/
(смотрите на даты файлов и качайте самую свежую).

Этот архив надо скачать, распаковать и запустить бат-файл run.bat

Запустится локальный мад-сервер, к которому можно получить доступ из мад-клиента по адресу
localhost 3000 (в зависимости от конфигурации вашей домашней/офисной сети возможно локальный мад
будет доступен и снаружи из Интернета по IP адресу вашей машины. А может и нет)

4. Для ОС Линукс есть "виндовый" подход, а именно уже откомпилированная сборка со статически прилинкованными
библиотеками, которая должна запуститься в любом дистрибутиве:

http://files.mud.kharkov.org/proolmud-static/

5. Но более правильный Linux way скачать данный дистрибутив и откомпилировать (собрать) его примерно так

cd src

make

(возможно придется поставить недостающих библиотек, см. ниже)

Потом

cd ..

bin/circle

И запустится локальный сервер

На это пути могут возникнуть подводные камни, так как скорее всего придется устанавливать библиотеки, отсутствующие
в вашем Линуксе. Здесь придется включить моск!

6. Другие ОС (FreeBSD, Android, macOS, MSDOS)

Многие другие ОС похожи на UNIX (например, Android, macOS) и там тоже есть компилятор gcc++ и команда make.
А вот для сборки в среде MSDOS потребуется очень сильное колдунство!

**Автозапуск**

Для автозапуска мада при загрузке компьютера в файл rc.local надо внести строку

su prool -c ~prool/proolmud/autorun.vmud & > /dev/null

Примечания: полные пути на вашем сервере могут быть немного другими

**Замечания по сборке**

Ubuntu 18.04:

sudo apt-get install gcc-multilib g++-multilib libz-dev libboost-all-dev

BOOST может понадобиться не из репозиториев ОС, а свежий, с www.boost.org (а может и нет!)

Для сборки в среде Ubuntu 15.10 64 bit надо удалить -lz из Makefile и закомментировать HAVE_ZLIB
в conf.h (вообще, при современных скоростях Интернета сжатие, именно для которого нужна zlib, практически не нужно)

И вот еще такое шаманство может потребоваться, если не найдется файл zconf.h

sudo ln -s /usr/include/x86_64-linux-gnu/zconf.h /usr/include

и флаг -L/usr/lib/x86_64-linux-gnu/libz.a вместо -lz

Ubuntu 20.04:

Кроме всего прочего понадобится sudo apt install ibcrypt-dev:i386

*Пруль*

http://prool.kharkov.org

http://mud.kharkov.org

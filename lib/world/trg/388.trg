#38800
заходим к руму 38823~
2 e 100
~
wait 1s
wecho _Одна из картин как-будто ожила и со стены сошло несколько теней.
wload mob 38818
wload mob 38818
calcuid oxboga 38822 room
exec 38827 %oxboga.id%
calcuid orxboa 38851 room
exec 38828 %orxboa.id%
calcuid torxboa 38824 room
exec 38828 %torxboa.id%
calcuid fertrum 38823 room
detach 38800 %fertrum.id%
~
#38801
репоп тригеров в зоне~
2 f 100
~
calcuid f1ertrum 38823 room
detach 38800 %f1ertrum.id%
calcuid ferstrum 38823 room
attach 38800 %ferstrum.id%
calcuid d1rakrom 38870 room
detach 38809 %d1rakrom.id% 
calcuid dra1k1rom 38852 room
detach 38809 %dra1k1rom.id% 
calcuid fe2rtrm 38802 room
detach 38802 %fe2rtrm.id%
calcuid fertrm 38802 room
attach 38802 %fertrm.id%
calcuid hello 38823 mob
attach 38812 %hello.id%
calcuid sqtollik 38802 obj
attach 38816 %sqtollik.id% 
calcuid tron 38840 room
attach 38810 %tron.id% 
calcuid kashey 38800 mob
attach 38817 %kashey.id% 
wdoor 38885 down purge
wdoor 38884 down purge
calcuid wfqerstr 38858 room
attach 38822 %wfqerstr.id%
calcuid fqeum 38820 room
attach 38823 %fqeum.id%
calcuid crypt 38885 room
detach 38820 %crypt.id%
attach 38820 %crypt.id%
calcuid crypt2 38884 room
detach 38820 %crypt2.id%
attach 38820 %crypt2.id%
wait 1
calcuid podval 38832 room
detach 38821 %podval.id%
attach 38821 %podval.id%
~
#38802
заходим к руму 38802~
2 e 100
~
wait 1s
wecho _Одна из картин как-будто ожила и со стены сошло несколько теней.
wload mob 38818
wload mob 38818
calcuid orxrbo 38801 room
exec 38827 %orxrbo.id%
calcuid otxtbo 38800 room
exec 38828 %otxtbo.id%
calcuid ytxtby 38803 room
exec 38828 %ytxtby.id%
calcuid fertrm 38802 room
detach 38802 %fertrm.id%
~
#38803
в бою с родительницей скелетов~
0 k 50
~
switch %random.6%
  case 1
    mecho _Родительница скелетов легко произвела на свет новорожденного скелета.
    mload mob 38806
  break
  case 2
    mecho _Родительница скелетов напряглась, и произвела на свет окрепшего скелета.
    mload mob 38807
  break
  case 3
    mecho _Родительница скелетов в судорогах произвела на свет скелета-война.
    mload mob 38808     
  break
  case 4
    mecho _Родительница скелетов изогнулась, и с диким воем родила скелета Ужаса.
    mload mob 38809
  break
  default
  break
done
end
~
#38804
спрыгнули вниз в руме 38809~
2 e 100
~
if %direction%==down
  wsend %actor% _Вы спрыгнули вниз и сообразили, что дыра, слишком высоко от Вас. 
  wsend %actor% _Пути назад нет.
end
~
#38805
лезть в щель~
2 c 0
лезть ползти пробираться~
if (%arg.contains(щель)%)||(%arg.contains(пещер)%)
  if (%actor.move%<40)
    wsend %actor% _У Вас не хватит сил на такой переход.
    return 0
  else
    wsend %actor% _Вы полезли в узкую щель.
    wechoaround %actor% _%actor.name% полез%actor.q% в узкую щель.
    wait 1s
    wsend %actor% _После длительного пути вы выбрались в просторное место.
    if (%actor.realroom%==38805)
      wteleport %actor% 38806 
      eval temp %actor.move(-40)%
    elseif (%actor.realroom%==38824)
      wteleport %actor% 38825
      eval temp %actor.move(-40)%
    endif
    wat %actor.realroom% wechoaround %actor% _%actor.name% пробрал%actor.u% сюда.
  end
end
end
~
#38806
лезть в щель назад~
2 c 0
лезть ползти пробираться~
if (%arg.contains(щель)%)||(%arg.contains(пещер)%)
  if (%actor.move%<40)
    wsend %actor% _У Вас не хватит сил на такой переход.
    return 0
  else
    wsend %actor% _Вы полезли в узкую щель.
    wechoaround %actor% _%actor.name% полез%actor.q% в узкую щель.
    wait 1s
    wsend %actor% _После длительного пути вы выбрались в просторное место.
    if (%actor.realroom%==38806)
      wteleport %actor% 38805 
      %actor.move(-40)%
    elseif (%actor.realroom%==38825)
      wteleport %actor% 38824
      %actor.move(-40)%
    endif
    wat %actor.realroom% wechoaround %actor% _%actor.name% пробрал%actor.u% сюда.
  end
end
~
#38807
нырнуть в подземную реку~
2 c 0
нырнуть плыть занырнуть~
if (%arg.contains(рек)%)||(%arg.contains(вод)%)
  if (%actor.move%<100)
    wsend %actor% _Что-то подсказывает Вам, что у Вас не хватит сил на весь путь.
    wsend %actor% _Вы нырнули в холодную воду и понеслись по быстрому потоку неведомо куда.
    wechoaround %actor% _%actor.name% полез%actor.q% в воду.
    wait 1s
    wsend %actor% _Скоро Вы осознали, что задыхаетесь и силы на исходе!!! Вы тонете!!!
    wteleport %actor% 38888 
    return 0
  else
    wsend %actor% _Вы нырнули в холодную воду и понеслись по быстрому потоку неведомо куда.
    wechoaround %actor% _%actor.name% полез%actor.q% в воду.
    wait 1s
    wsend %actor% _После длительного пути вы выбрались на сухое место и отряхнулись.
    if (%actor.realroom%==38857)
      wteleport %actor% 38881 
      eval temp %actor.move(-100)%
    elseif (%actor.realroom%==38815)
      wteleport %actor% 38816
      eval temp %actor.move(-100)%
    endif
    wat %actor.realroom% wechoaround %actor% _%actor.name% пробрал%actor.u% сюда из воды, отплевываясь и задыхаясь.
  end
end
end
end
~
#38808
нырнуть в подземную реку назад~
2 c 0
нырнуть плыть занырнуть~
if (%arg.contains(рек)%)||(%arg.contains(вод)%)
  wsend %actor% _Вы попытались нырнуть в воду, но течение противное и мешает.
  wsend  %actor% _Становится ясно, что уплыть назад невозможно.
  wechoaround %actor% _%actor.name% полез%actor.q% в воду, но мощный поток помешал уплыть отсюда.
  return 0
end
~
#38809
лезть в окно~
2 c 0
лезть ползти пробираться~
if (%arg.contains(окно)%)||(%arg.contains(вперед)%)
  if (%actor.move%<10)
    wsend %actor% _У Вас не хватит сил для этого.
    return 0
  else
    wsend %actor% _Вы полезли в смотровое окно.
    wechoaround %actor% _%actor.name% полез%actor.q% в окно.
    wait 1s
    wsend %actor% _Вы выбрались во внутренний дворик замка.
    if (%actor.realroom%==38852)
      wteleport %actor% 38878 
      eval temp %actor.move(-10)%
    elseif (%actor.realroom%==38870)
      wteleport %actor% 38879
      eval temp %actor.move(-10)%
    endif
    wat %actor.realroom% wechoaround %actor% _%actor.name% пробрал%actor.u% сюда.
  end
end
end
~
#38810
двигаем трон~
2 c 0
двигать сдвинуть двинуть~
if (%arg.contains(трон)%)||(%arg.contains(кресло)%)
  if (%actor.move%<90)
    wsend %actor%  У Вас не хватит сил для этого.
    return 0
  else
    wsend %actor%  Вы сдвинули трон.
    eval buffer %actor.move(-90)%
    wechoaround %actor%  %actor.name%, напрягшись, сдвинул%actor.g% трон.
    *if %random.100% < 75
    wload obj 38810
    *else
    *wecho   ...но под ним ничего не оказалось.
    *end
    calcuid tron 38840 room
    detach 38810 %tron.id% 
  end
end
~
#38811
в бою с кащеем~
0 k 35
~
switch %random.15%
  case 1
    msend %actor% Кость, которую метнул Кащей, попала Вам в грудь!
    mechoaround %actor% Кость, которую метнул Кащей, вонзилась %actor.dname% в грудь!
    mdamage %actor% 200
    крут
  break
  case 2
    msend %actor% Кость, которую метнул Кащей, попала Вам в глаз!
    mechoaround %actor% Кость, которую метнул Кащей, вонзилась %actor.dname% в глаз!
    mdamage %actor% 100
    пальц
  break
  case 3
    msend %actor% Кость, которую метнул Кащей, попала Вам в живот и сбила с ног!
    mechoaround %actor% Кость, которую метнул Кащей, вонзилась %actor.dname% в живот и сбила с ног!
    %actor.position(6)%
    %actor.wait(2)%
    mdamage %actor% 100
    ул
  break
  case 4
    mecho Взмахом руки Кащей Бессмертный послал огненный шар!!!
    foreach target %self.pc%
      msend %target% Огненный шар, посланный Кащеем, опалил Вас с ног до головы!
      mechoaround %target%  Огненный шар, посланный Кащеем, опалил %target.rname% с ног до головы!
      mdamage %target% 250
    done
  done
~
#38812
помочь старушке~
0 d 0
здравствуйте здравствуй здорово поклон~
wait 1s
say Здорово, коли не шутишь..
wait 1s
пла
calcuid helslo 38823 mob
attach 38813 %helslo.id%
calcuid hello 38823 mob
detach 38812 %hello.id%
end
~
#38813
помочь старушке 2~
0 d 0
смогу помогу чем помочь как тебе помочь~
if %exist.obj(38811)%
  wait 1s
  say Спасибо большое... но мне уже помогают люди добрые.
  ул
  return 0
else
  wait 2s
  say Ой, уж не верю я, что помочь мне кто-то может.
  wait 1s
  пла
  wait 1s
  say Ой, несчастная я! Ой горе-то горюшко...о-о-о.
  wait 1s
  пла
  wait 2s
  say Потеряла я внучку единственную-у-у-у. Дура я старая-а-а-а.
  wait 1s
  пла
  wait 1s
  say И что делать-то не знаю-у-у-у..
  wait 1s
  пла
  wait 1s
  say А потерялась она, когда пошла гулять в лесок соседний, что на западе.
  wait 3s
  дум 
  wait 1s
  say Да, говорят, поймали ее люди черные, злые. А-а-а-а.
  wait 1s
  пла
  wait 1s
  say Ой, благословляю тебя, храбрая душа...
  ул
  wait 1s
  say Может она и не жива уже, моя лебедушка...
  wait 1s
  дум
  wait 1s
  say Тогда хоть принеси ее останочки... Я похороню их по християнски...
  wait 1s
  wait 1s
  say Говорили люди знающие, что останки ее могут быть у чернокнижников, в гробницах, что под склепками хранятся...
  wait 1s
  пла
  mload obj 38811
  дать крест %actor.name%
  say Вот.. возьми ее крестик... Пусть он будет тебя направлять в пути и помогать.
  calcuid hello 38823 mob
  detach 38813 %hello.id%
end
~
#38814
помочь старушке 3~
0 d 0
помоги поясни~
calcuid heqllo 38823 mob
detach 38814 %heqllo.id%
end
~
#38815
родительница скелетов померла~
0 f 100
~
if (%world.curobjs(38818)% <1) && (%random.2% == 1)
  mload obj 38818
end
if (%world.curobjs(569)%==0) && (%random.6%==1)
  mload obj 569
end
mecho _Из трупа родительницы скелетов вывалилось несколько недоносков.
mload mob 38806
mload mob 38807
mload mob 38808
mload mob 38809
mload mob 38806
~
#38816
кто то трахает гробницу~
1 p 50
~
wait 1s
oecho _Из гробницы медленно выполз дух погребенного чернокнижника.
oload mob 38824
detach 38816 %self.id%
~
#38817
в бою с кащеем если у него меньше 500 хп~
0 l 100
~
if %self.hitp% < 600
  mecho _Кащей Бессмертный свистом призвал верхового Дракона.
  mload mob 38821
  calcuid kassshey 38800 mob
  attach 38818 %kassshey.id% 
  calcuid kashey 38800 mob
  detach 38817 %kashey.id% 
end
~
#38818
в бою с кащеем если у него меньше 100 хп~
0 l 100
~
if %self.hitp% < 150
  mecho _Кащей Бессмертный сообразил наконец, что продолжать бой бесполезно.
  calcuid drakon 38821 mob
  attach 38819 %drakon.id% 
  exec 38819 %drakon.id%
  calcuid kawshey 38800 mob
  detach 38818 %kawshey.id% 
end
~
#38819
дракон уносит кащея~
0 z 100
~
wait 1
if (%self.realroom%==38832)
  mecho Дракон взмахом крыльев отворил тайный ход в крыше...
  mecho Дракон подхватил умирающего Кащея на спину и был таков.
  calcuid werdrakon 38800 mob
  mteleport %werdrakon% 38886
  calcuid drakon 38821 mob
  calcuid podval 38832 room
  detach 38821 %podval.id%
  mpurge %drakon%
end
~
#38820
с крестом в подсклепье~
2 c 0
освятить поцеловать осенить помахать махать угрожать трясти~
if !%arg.contains(крест)% && !%arg.contains(склеп)%
  halt
end
if !%actor.haveobj(38811)%
  wsend %actor% У вас нет этого.
  halt
end
wsend %actor%  Вы освятили крестиком помещение.
wechoaround %actor%  %actor.name% освятил%actor.g% крестиком склеп...
wait 1
wecho Внутренняя сила крестика рассеяла тьму.
if (%actor.realroom%==38885)
  wdoor 38885 down flags ab
  wdoor 38885 down room 38821  
elseif (%actor.realroom%==38884)
  wdoor 38884 down flags ab
  wdoor 38884 down room 38848  
end
detach 38820 %self.id%
~
#38821
чар входит к кащею~
2 e 90
~
wait 1
if (%world.curmobs(38800)% > 0)
  eval chanse %random.10%*(%actor.int%+%actor.intadd%)
  if %chanse% > 92
    halt
  end
  wsend %actor% Вы потрясены ужасным видом Кащея Бессмертного!
  wechoaround %actor% %actor.name% потрясен%actor.g% ужасным видом Кащея Бессмертного!
  %actor.position(6)%
  %actor.wait(3)%
end
~
#38822
двигаем рычаг 1~
2 c 0
дернуть тянуть сдвинуть повернуть~
if (%arg.contains(рычаг)%)||(%arg.contains(рычажок)%)
  wsend       %actor% _Вы сдвинули рычаг.
  wechoaround %actor% _%actor.name%, напрягшись, сдвинул%actor.g% рычаг.
  calcuid pqervi 38852 room
  exec 38825 %pqervi.id%
  calcuid j145skejvi 38828 mob
  wpurge %j145skejvi%
  calcuid sqervi 38819 room
  exec 38825 %sqervi.id%
  calcuid rqervi 38818 room
  exec 38825 %rqervi.id%
  calcuid fqerstrum 38858 room
  detach 38822 %fqerstrum.id%
end
~
#38823
двигаем рычаг 2~
2 c 0
дернуть тянуть сдвинуть повернуть~
if (%arg.contains(рычаг)%)||(%arg.contains(рычажок)%)
  wsend       %actor% _Вы сдвинули рычаг.
  wechoaround %actor% _%actor.name%, напрягшись, сдвинул%actor.g% рычаг.
  calcuid pqwervi 38870 room
  exec 38825 %pqwervi.id%
  calcuid j14skejvi 38827 mob
  wpurge %j14skejvi%
  
  calcuid tqervi 38869 room
  exec 38825 %tqervi.id%
  calcuid yqervi 38868 room
  exec 38825 %yqervi.id%
  calcuid fqeum 38820 room
  detach 38823 %fqeum.id%
end
~
#38824
дамаджим драконов~
0 z 100
~
mecho _Дракон только крякнул под тяжестью каменного шара.
mpurge %self.id%
~
#38825
дамадж всех в руме~
2 z 100
~
wecho _Откуда-то свалился огромный каменный шар.
wecho _Шар понесся прямо на Вас, уничтожая все на пути.
foreach victim %self.all%
  if !%victim%
    halt
  end
  wdamage %victim% 1000
done
~
#38826
помер кащей~
0 f 100
~
if %world.curobjs(1254)% <1 &&  %world.curobjs(1291)% <1
  if %world.curobjs(1255)% <1
    if %random.100% < 3
      mload obj 1254
    end
  end
end
if (%world.curobjs(38819)% <1) && (%random.1000% <= 200)
  mload obj 38819
end
if (%world.curobjs(38821)% <2) && (%random.1000% <= 200)
  mload obj 38821
end
if (%world.curobjs(567)%==0) && (%random.1000% <= 160)
  mload obj 567
end
if (%world.curobjs(565)%==0) && (%random.1000% <= 160)
  mload obj 565
end
if (%world.curobjs(38822)% <2) && (%random.1000% <= 200)
  mload obj 38822
end
if (%world.curobjs(226)% <20) && (%random.1000% <= 200)
  mload obj 226
end
~
#38827
запускаем лоад ож.богатыря~
2 z 100
~
wait 1s
wecho _Одна из гравюр как-будто ожила и со стены сошел оживший человек.
wload mob 38818
~
#38828
запускаем лоад ожившего странника~
2 z 100
~
wait 1s
wecho _Одна из гравюр как-будто ожила и со стены сошло несколько теней.
wload mob 38819
wload mob 38819
~
#38829
ловкий убегает~
0 k 30
~
бежать
бежать 
wait 1s
оглян
~
#38830
помер васииск~
0 f 100
~
if (%world.curobjs(559)%==0) && (%random.3%==1)
  mload obj 559
end
~
#38831
чар лезет в окно кога драк в игре~
0 c 0
лезть ползти пробираться пролезть~
if (%arg.contains(окно)%)||(%arg.contains(вперед)%)
  if (%actor.move%<10)
    wsend %actor% _У Вас не хватит сил для этого.
    return 0
  else
    wsend %actor% _Вы полезли в смотровое окно.
    wechoaround %actor% _%actor.name% полез%actor.q% в окно.
    wait 1s
    wsend %actor% _Однако огромный дракон схватил Вас зубами, как козявку, и выкинул подальше.
    wechoaround %actor% _%actor.name% полез%actor.q% в окно, но огромный дракон пинком ноги отшвырнул %actor.vname% подальше.
    if (%actor.realroom%==38852)
      wteleport %actor% 38819 
    elseif (%actor.realroom%==38870)
      wteleport %actor% 38869
    endif
  end
end
end
~
#38832
помер вел.дракон 38827~
0 f 100
~
calcuid drakrom 38870 room
attach 38809 %drakrom.id% 
~
#38833
помер вел.дракон 38828~
0 f 100
~
calcuid dra1krom 38852 room
attach 38809 %dra1krom.id% 
~
$~

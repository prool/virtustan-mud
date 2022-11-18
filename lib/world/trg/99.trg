#9900
Зашли к деду-квестеру~
0 r 100
~
If %exist.obj(9986)% == 1
  wait 1s
  say Ох-ох-ох, ноють старые кости. Стар я стал и видеть стал плохо, и все забываю.
  wait 1s
  say А вчера где-то гулял на юге и потерял свои очки: то ли на лугах, то ли в пустыне.
  wait 1s
  say Помоги мне! Найди мои очки, а я в долгу не останусь.
else
  Halt
end
~
#9901
Сдали предмет деду-квестеру~
0 j 9986
100~
wait 1s
if %object.vnum% == 9986
  say Благодарствую, ты меня выручил%actor.g%. Теперь я вижу все! 
  switch %random.3%
    case 1
      say Вот тебе награда за это.
      mload obj 9946
      give кираса .%actor.name%
    break
    case 2
      say В награду я дам тебе 150 монет, если найду в своих карманах
      %self.gold(+150)%
      give 150 вирт %actor.name%
    break
    case 3
      say К сожалению, мне нечем тебя наградить.
    break
  done
  mpurge очки
else
  say Ась?
  wait 2s
  say Что это?! Ничего не вижу без очков!
  wait 1s
  eval getobject %object.name%
  if %getobject.car% == труп
    mpurge труп
  else
    брос %getobject.car%.%getobject.cdr%
  end
end
~
#9902
Черный альпинист~
0 d 100
привет~
say Если ты, путник, поднимешься на вершину великой горы Джомолунгмы
say (в реале) и принесешь мне оттуда кусочек камня, я тебя озолочу.
say Ха-ха-ха! Иди же!
done
~
#9903
прапор~
0 gh 100
~
гов слава Виртустану, слава королю!
%echo% %self.name% отдал%actor.g% честь
~
#9904
казино~
2 c 100
играть~
if !%arg%
  %send% %actor% играть шанс ставка
  %send% %actor% игра происходит на куны, выигрыш расчитывается так: ставках100/шанс
  %send% %actor% внимание: казино не поддерживает дробных чисел
  halt
end
set chance %arg.car%
set bet %arg.words(2)%
if %chance% > 95 || %chance% < 1
  %send% %actor% шанс выигрыша не может быть выше 95 и ниже 1
  halt
end
if %bet% < 1
  %send% %actor% ставка не может быть ниже 1
  halt
end
wat 9999 %echo% %actor.gold(-%bet%)%
eval rnd %random.100%
if %rnd% <= %chance%
  eval vin (%bet%*100)/%chance%
  eval vin2 %vin%-%bet%
  %send% %actor% вы выиграли!!!
  %send% %actor% выигрыш: %vin%, чистая прибыль: %vin2%, стало кун: %actor.gold(+%vin%)%
else
  %send% %actor% вы проиграли %bet% кун
  %send% %actor% ничего, в следующий раз повезет.
end
~
#9905
уничтожить Харьков, немедленно!~
2 f 100
~
if %random.100% <= 30
  eval event %random.100%
  if %event% <= 70
    eval firstvnum 9700
    eval lastvnum 9997
    eval counter %firstvnum%
    while %counter% <= %lastvnum%
      eval room %world.room(%counter%)%
      if %room%
        foreach j %room.objects%
          if %j.vnum% == 9995
            eval flag 1
          end
        done
        if !%flag% && %random.100% <= 15
          wat %counter% %load% obj 9995
        end
      end
      unset flag
      wait 2
      eval counter %counter%+1
    done
    if %random.100% <= 80
      wzoneecho 9900 &RВнимание, харьковчани! Разведка Виртустана установила проведение вражескими силами диверсии, заключающейся в минировании города.&n
      wzoneecho 9900 &RБудте крайне осторожны и старайтесь не покидать своих домов до специального оповещения.&n
    end
  end
end
~
#9906
нарвались на растяжку~
1 t 100
~
%send% %actor% Вы неосторожно задели установленную здесь растяжку!!!
%echoaround% %actor% %actor.iname% неосторожно задел%actor.g% установленную здесь растяжку!!!
%echo% &RУжасающий по мощности взрыв сотряс землю!&n
foreach i %self.all%
  %send% %i% Мощнейшим взрывом вас убило на месте!
  %echoaround% %i% Мощнейшим взрывом %i.vname% убило на месте!
  eval dmg %i.hitp%+20
  %damage% %i% %dmg%
done
%purge% %self%
~
#9950
телепорт к полянам~
2 c 100
поляни~
wait 1
%portal% 7028 1
~
#9951
телепорт на остров~
2 c 100
остров~
wait 1
%portal% 21038 1
~
#9989
перемещение~
0 d 100
телепорт~
г Извини, но сейчас я не могу тебя телепортировать куда либо
~
#9990
приветствие старика~
0 h 100
~
wait 1s
г Привет, %actor.name%
wait 1s
г Если ты хочешь куда-нибудь переместиться, то скажи точку прибытия.
wait 1s
г Если тебя нужны защитные заклинания, то так и скажи
wait 1s
улы .%actor.name%
~
#9991
делаем обкаст~
0 d 100
защитные~
if (!%arg%.contains(закл))
  г Гм, я не расслышал, повтори еще
  halt
end
г Щас, %actor.name% все сделаем в лучшем виде !
wait 1s
emo зашептал древние слова
dg_cast 'защи' %actor.name%
dg_cast 'затум' %actor.name%
dg_cast 'кам кож' %actor.name%
dg_cast 'мига' %actor.name%
dg_cast 'внима' %actor.name%
dg_cast 'полет' %actor.name%
dg_cast 'невид' %actor.name%
dg_cast 'наст' %actor.name%
dg_cast 'сила' %actor.name%
dg_cast 'кош лов' %actor.name%
dg_cast 'быч тел' %actor.name%
dg_cast 'возд щи' %actor.name%
dg_cast 'огне щи' %actor.name%
dg_cast 'лед щит' %actor.name%
wait 1s
улы
г Ну вот и всё
пок %actor.name%
~
#9992
триг к мозгу~
1 bz 100
~
done
wait 1
switch %random.12%
  Case 1
    oecho Из глубины мозга послышалось гудении: "Туууууууу"
  break
  case 2
    oecho Вдруг мозг Бодрича авторитетно заявил: "А Младин не выйграет олимпиаду!"
  break
  case 3
    oecho "Я гений", - удовлетворенно подумал про себя мозг Бодрича,
    oecho Делая новые элементы таблицы Менделеева
  break
  case 4
    oecho "Мы не кочегары, мы не плотники...", - пропел Бодрич, запуская коллайдер
  case 5
    oecho Мозг Бодрича, в невыразимой тоске, создал скелета, поменял руки,
    oecho ноги у скелета, и расхохотался.
  break
done
~
#9993
new trigger~
0 c 100
каст~
mecho Кродо в гневе обрек вас на смерть!
dg_cast 'суд богов'
done
~
#9994
prool trig 1~
2 d 100
эхо~
say My trigger commandlist is not complete!
say Да, эхо!
~
$~

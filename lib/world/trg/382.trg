#38200
помер перегуд~
0 f 100
~
if %world.curobjs(38214)% == 0
  mload obj 38214
end
~
#38201
помер перекраса~
0 f 100
~
if %world.curobjs(38215)% == 0
  mload obj 38215
end
~
#38202
топить печь~
2 c 0
топить растопить~
if !(%arg.contains(печь)%) 
  wsend       %actor% Что вы хотите растопить???
  return 0
  halt
end
wsend %actor% Вы подкинули дровишек в печь и огонь запылал ярче.
wechoaround %actor% %actor.name% подкинул%actor.g% дровишек в печь. Огонь запылал ярче.
wait 1
wecho _С диким криком из печи вывалился ошпаренный трубочист.
wload mob 38231
wait 2
wecho _"Как больно!" - заорал трубочист, осматриваясь в поисках обидчика.
calcuid pechnikr 38232 room
detach 38202 %pechnikr.id%
~
#38203
репоп тригеров~
2 f 100
~
calcuid pechnikr 38232 room
attach 38202 %pechnikr.id%
wdoor 38245 up purge
calcuid stol 38245 room
attach 38204 %stol.id%
calcuid qstoql 38233 mob
detach 38207 %qstoql.id%
attach 38206 %qstoql.id%
~
#38204
двигать стол~
2 c 0
двигать двинуть~
if !(%arg.contains(стол)%) 
  wsend %actor% Что вы хотите сдвинуть???
  return 0
  halt
end
wsend %actor% Вы пододвинули стол к стене...
wechoaround %actor% %actor.name% пододвинул%actor.g% стол к стене.
wait 1
wdoor   38245 up flags ab
wdoor   38245 up room  38291 
wecho Стало возможно пройти наверх.
calcuid stol 38245 room
detach 38204 %stol.id%
~
#38205
помер волхв~
0 f 100
~
if ( %world.curobjs(211)% < 20 ) && ( %random.5% == 1 )
  mload obj 211
end
if (%random.10% == 1)
  mload obj 549
end
~
#38206
даем шапку купцу~
0 j 100
~
if %object.vnum% == 38218 
  wait 2s
  mpurge шапк
  wait 2s
  say Спасибо. Выручил%actor.g%! Я давно искал свою шапку...
  msend       %actor% _За доброе дело Вы получили 5000 очков опыта...
  %actor.exp(+5000)%  
  wait 2s
  say В нонешние времена не легко найти хорошего человека.
  wait 2s
  say Все они погрязли в душегубстве и разгуле...
  взд
  wait 1s
  say Но, ты вроде не из таких чтоб токо по кабакам да по битвам..
  wait 1s
  say Сможешь мне помочь? Отблагодарю..
  calcuid stoql 38233 mob
  attach 38207 %stoql.id%
end
else
say  Зачем мне это? 
eval getobject %object.name%
брос %getobject.car%.%getobject.cdr%
end
~
#38207
гриш помогу купцу~
0 d 0
да готов согласен помогу согласна~
wait 1s
say Вот и ладушки, %actor.name%.
say Есть у меня брат. Тоже купец из видных, первостатейных.
wait 1s
say Не видал я его давно, и вестей от него нет вот уже почитай как два годика.
wait 1s
say Отправился он с кораблем на большой остров, что в Окияне, с тамошними торговать.
say Да , не вернулся что-то. Вот если бы ты его разыскал%actor.g% и вот эту весточку передал%actor.g%.
wait 1s
mecho _Купец быстро начеркал что-то на куске кожи..
mload obj 38222
дат весточк %actor.name%
wait 1s
say Вот бы мы тебя и отблагодарили.
жда
calcuid stoql 38233 mob
detach 38207 %stoql.id%
~
#38208
развязуем купца~
0 c 0
развязать освободить~
if !(%arg.contains(купец)%) 
  msend %actor% Кого Вы хотите освободить???
  return 0
  halt
end
wait 1
msend %actor% Вы развязали купца и выкинули веревки.
mechoaround %actor% %actor.name% развязал%actor.g% купца.
mecho Купчина очень обрадовался и стал Вас благодарить!
if %actor.level% > 20
  msend %actor%  За доброе дело Вы получили 80000 очков опыта...
  %actor.exp(+80000)%  
end
say Я пойду, расскажу хозяйке, что Вы меня освободили!!!
mecho Купчина еще раз поклонился и ушел.
wait 1
mpurge %self%
~
$~

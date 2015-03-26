#33200
у 5 русалок~
0 q 100
~
wait 1
say Мы охраняем вход в Подводное царство от нашествия мерзких спрутов!
say и Вас мы тоже не пропустим!
~
#33201
дал утробу русалке~
0 j 100
~
wait 1
if %object.vnum% == 33105 
  wait 1
  %purge% %object%
  wait 2s
  say Ты отомстил за мою сестру?!! Молодец...
  wait 2s
  mecho Русалка кинулась Вам на шею и горячо отблагодарила.
  say Проходи, добрый путник, пусть тебя омывают только теплые течения!
  foreach char %self.char%
    mechoaround %char% %char.name% был%char.g% пропущен%char.g% дальше!!! 
    mteleport %char% 33202
    msend %char% .- Вы пропущены дальше!
  done
end
~
#33202
дал лук русалке~
0 j 100
~
if %object.vnum% == 33110 then
  wait 1s
  say Убийца! Так это ты убил нашу подругу!
  mkill %actor.name%
end
~
#33203
нырнуть пещера~
2 c 0
нырнуть занырнуть~
if !(%arg.contains(грот)%) 
  wsend       %actor% Куда это Вы хотите нырнуть???
  return 0
  halt
end
if %actor.fighting%
  wsend %actor% Вы не можете нырнуть пока сражаетесь.
end
wsend       %actor% Вы бесстрашно нырнули в пещеру.
wechoaround %actor% %actor.name%, превратившись в рыбу, уплыл%actor.g% по течению вниз.
wait 1s
wsend %actor.name% .- Вы оказались в странной пещере...
wat 33300 wecho Кто-то приплыл сюда.
wteleport %actor.name% 33300
end
~
#33204
у призрака торговца~
0 q 100
~
wait 1
mecho _Призрак торговца нагнулся, встал на колени и стал старательно обнюхивать землю
wait 1s
say Где же вы мои денежки... Где же вы мои лапоньки...
плак
wait 2s 
mecho _Призрак торговца увидел Вас, поднялся
wait 1s
say Ах, если бы ты принес мне мой мешок с деньгами, я наконец-то смог бы упокоиться.
wait 2s
say И может быть, открыл бы тебе тайный вход к Царю морскому!
wait 1s
mecho _Призрак торговца снова встал на колени и стал старательно обнюхивать палубу
~
#33205
у капитана~
0 q 100
~
wait 1
mecho  _Призрак капитана Николаидиса задумался, посмотрел на Вас, снова задумался
wait 1s
say "Венеция"...? - нет, "Иди к черту"..? - нет,
вздох
wait 2s 
mecho _Призрак капитана стал загибать пальцы, считая что-то
wait 1s
say "Бог с вами"...? - нет, "Виктория"...?  - нет...
wait 2s
say Ну как же назывался мой корабль....
calcuid konek 33215 mob
detach 33205 %konek.id%
~
#33206
в бою со скелетом~
0 q 100
~
wait 4s
mecho _От Вашего мощного удара скелет рассыпался по косточкам
mecho _Однако кости быстро собрались в кучу и скелет продолжил бой
~
#33207
прыгнуть~
2 c 0
прыгнуть перепрыгнуть~
if !(%arg.contains(разлом)%) 
  wsend       %actor% Куда это Вы хотите прыгнуть???
  return 0
  halt
end
wsend       %actor% Разогнавшись, вы с трудом прыгнули через разлом.
wechoaround %actor% %actor.name% с трудом перепрыгнул%actor.g% разлом!
wait 1s
wsend %actor.name% .- Вы с огромным трудом перепрыгнули на другую половину корабля
wteleport %actor.name% 33271
wat 33271 wechoaround %actor% Кто-то приплыл сюда.
end
~
#33208
плыть борт~
2 c 0
плыть переплыть~
if !(%arg.contains(борт)%) 
  wsend       %actor% Куда это Вы хотите плыть???
  return 0
  halt
end
wsend       %actor% Вспомнив любимую рыбку, вы быстро начали барахтаться...
wechoaround %actor% %actor.name% поплыл%actor.g% к другой части корабля.
wait 1s
wsend %actor.name% - Вам не пришлось долго плыть до корабля, и вот вы уже там!
wteleport %actor.name% 33290
wat 33290 wechoaround %actor% Кто-то приплыл сюда.
end
~
#33209
карабкаться~
2 c 0
карабкаться~
if !(%arg.contains(вверх)%) 
  wsend       %actor% Куда это Вы хотите карабкаться???
  return 0
  halt
end
wsend       %actor% Как краб, вы ловко вскарабкались вверх.
wechoaround %actor% %actor.name% двинул%actor.g% вверх.
wait 1s
wsend %actor.name% .- Вы лихо вскарабкались к обломкам корабля!
wteleport %actor.name% 33246 horse
wechoaround %actor% Кто-то вскарабкался сюда.
end
~
#33210
спуститься мачта~
2 c 0
спуститься опуститься держаться~
if !(%arg.contains(мачта)%) 
  wsend       %actor% Куда это Вы хотите спуститься???
  return 0
  halt
end
wsend       %actor% Держась за мачту, вы поплыли к кораблю.
wechoaround %actor% %actor.name% поплыл%actor.g% к кораблю, держась за мачту.
wait 1s
wsend %actor.name% .- Как гибкий уж, вы быстро спустились вдоль мачты.
wteleport %actor.name% 33287
wat 33287 wechoaround %actor% Кто-то спустился сюда.
end
~
#33211
нырнуть рея~
2 c 0
нырнуть занырнуть~
if !(%arg.contains(рея)%) 
  wsend       %actor% Куда это Вы хотите нырнуть???
  return 0
  halt
end
wsend       %actor% Отодвинув рею, вы нырнули в неизвестность.
wechoaround %actor% %actor.name% отодвинул%actor.g% рею и нырнул%actor.g% в неизвестность
calcuid brat 33213 mob
detach 33215 %brat.id%
attach 33214 %brat.id%
wait 1s
wsend %actor.name% .- Вы оказались в нешироком уютном лазе.
wteleport %actor.name% 33307 horse
wechoaround %actor% Кто-то занырнул сюда.
end
~
#33212
дал мешки купцу~
0 j 100
~
if %object.vnum% == 33204 then
  wait 2s
  say Ну что-ж, спасибо и на этом! 
  wait 2s
  say Скажу тебе, так и быть, что.... что...
  wait 2s
  say Хе-х, что вход в Подводное Царство - это огромный грот, что неподалеку!
  mecho Купец попытался упокоиться, но у него ничего не получилось...
  взд
  calcuid varo 33204 obj
  mpurge %varo.name%
end
if %object.vnum% == 33205 then
  wait 2s
  say Во, это то что мне нужно! 
  wait 2s
  say Вход в Подводное Царство расположен... честно говоря - не знаю где.
  wait 2s
  say Но ты не огорчайся!!! На тебе кольцо, отдай его моему брату-воину, может  быть он тебе поможет..
  mload obj 33206
  дат кол %actor.name%
  mecho Купец по быстренькому упокоился, прихватив с собой мешок с золотом.
  calcuid var 33205 obj
  mpurge %var.name%
  calcuid pokoi 33212 mob
  mpurge %pokoi.name%
  calcuid brat 33213 mob
  detach 33214 %brat.id%
  attach 33215 %brat.id%
end
~
#33213
в бою с змеем~
0 r 100
~
mecho _Змей Раздулся у Вас на глазах, высунул раздвоенный язычок и ринулся в атаку
убить %actor.name%
~
#33214
помер ураган~
0 f 100
~
if (%world.curobjs(506)% < 2) && (%random.1000% <= 250)
  mload obj 506
end
~
#33215
у седого война c кольцoм~
0 q 100
~
mecho _Седой воин недовольно взглянул на Вас
wait 1s
say Ну, сказывай быстро, зачем пришел! Какое дело ко мне есть?
~
#33216
дал седому кольцо~
0 j 100
~
if %object.vnum% == 33206 then
  wait 2s
  say Ты от брата!!! Как он там!!! Ах, спасибо!!!
  wait 2s
  mecho Седой воин разрыдался, вспомнив любимого брата...
  wait 1s
  say Открою тебе секрет... Вход в Подводное царство завален реей моего корабля!
  say Попробуй  занырнуть за рею...
  calcuid wchod 33257 room
  detach 33211 %wchod.id%
  halt
end
~
#33217
сказал "GOOD LUCK"~
0 d 1
Good Luck~
if (%actor.vnum% != -1)
  halt
end
wait 1
say Точно! Именно "Good Luck"
wait  2s
mecho Николаидис снова опечалился, задумался...
wait 1s
say Теперь я все вспомнил! Это он погубил мой корабль и меня! ОН!
mecho Призрак почесал затылок, задумчиво посмотрел вверх...
wait 2s
say Он - это точно! Но кто он? Не помню....
say Может быть ты найдешь его? Да... Ты выглядишь достаточно крепким...
say Я могу послать тебя в прошлое, что бы ты предотвратил предательство и мою смерть...
say За себя я не беспокоюсь... А вот жена!
say Хочешь ли ты помочь мне?
*calcuid teleporter 33215 mob
attach 33218 %self.id%
*calcuid t1eleporter 33215 mob
detach 33217 %self.id%
end
~
#33218
сказал "да"~
0 d 1
да конечно готов хочу~
if (%actor.vnum% != -1)
  halt
end
wait 1
say Ну что-ж, лети! Только вернуться ты сможешь лишь сказав возле вечного камня магические слова возврата...
mecho Николаидис поднял руки, махнул...
wait  1s
eval  firstchar %self.people%
eval  num 0
while %firstchar% && (%num% < 5)
  set pc %firstchar.next_in_room%
  if %firstchar.vnum% == -1
    mechoaround %firstchar% %firstchar.name% был%actor.g% отправлен%actor.g% в ПРОШЛОЕ!!! 
    mteleport %firstchar% 33483
    wait 1
    if %firstchar.realroom% == 33483
      msend %firstchar% .- Вы уже в прошлом! Как интересно!
      eval  num %num%+1
    end 
  end
  if %pc%
    makeuid firstchar %pc.id%
  else
    set firstchar 0
  end
done
calcuid varru 33414 room
attach 33400 %varru.id%
calcuid kartogr 33417 mob
attach 33405 %kartogr.id%
calcuid wdowa 33486 room
attach 33407 %wdowa.id%
calcuid tract 33401 mob
attach 33402 %tract.id%
*calcuid teleporter 33215 mob
detach 33218 %self.id%
end
~
#33219
прыгнуть~
2 c 0
прыгнуть перепрыгнуть~
if !(%arg.contains(разлом)%) 
  wsend       %actor% Куда это Вы хотите прыгнуть???
  return 0
  halt
end
wsend       %actor% Разогнавшись, вы с трудом прыгнули через разлом.
wechoaround %actor% %actor.name% с трудом перепрыгнул%actor.g% разлом.
wait 1s
wsend %actor.name% .- Вы с огромным трудом перепрыгнули на другую половину корабля
wteleport %actor.name% 33270
wat 33270 wechoaround %actor% Кто-то появился здесь .
end
~
#33220
плыть борт~
2 c 0
плыть переплыть~
if !(%arg.contains(борт)%) 
  wsend       %actor% Куда это Вы хотите плыть???
  return 0
  halt
end
wsend       %actor% Вспомнив любимую рыбку, вы быстро начали барахтаться...
wechoaround %actor% %actor.name% поплыл%actor.g% к другой части корабля.
wait 1s
wsend %actor.name% .- Вам не пришлось долго плыть до корабля и вот вы уже там!
wteleport %actor.name% 33283
wat 33283 wechoaround %actor% Кто-то приплыл сюда.
end
~
#33221
карабкаться~
2 c 0
карабкаться ~
if !(%arg.contains(вниз)%) 
  wsend       %actor% Куда это Вы хотите карабкаться???
  return 0
  halt
end
wsend       %actor% Как краб, вы ловко опустились вниз.
wechoaround %actor% %actor.name% двинул%actor.g% вниз.
wait 1s
wsend %actor.name% .- Вы лихо спустились вниз!
wteleport %actor.name% 33245
wat 33245 wechoaround %actor% Кто-то вскарабкался сюда.
end
~
#33222
подняться мачта~
2 c 0
проплыть плыть двигаться ~
if !(%arg.contains(мачта)%) 
  wsend       %actor% Куда это Вы хотите плыть???
  return 0
  halt
end
wsend       %actor% Держась за мачту, вы поплыли вверх.
wechoaround %actor% %actor.name% поплыл%actor.g% вверх, держась за мачту.
wait 1s
wsend %actor.name% .- Как гибкий уж, вы быстро поднялись вдоль мачты.
wteleport %actor.name% 33262
wat 33262 wechoaround %actor% Кто-то поднялся сюда.
end
~
#33223
вход в квест ДОРОГА В ПЦ 2~
2 f 100
~
calcuid brat 33213 mob
detach 33215 %brat.id%
attach 33214 %brat.id%
calcuid konek 33215 mob
attach 33205 %konek.id%
calcuid t1eleporter 33215 mob
attach 33217 %t1eleporter.id%
calcuid teleporter 33215 mob
attach 33218 %teleporter.id%
~
#33224
дал карту призраку~
0 j 100
~
wait 1
if %object.vnum% == 33417 then
  wait 2s
  say Ага! Это карта рифов, с неуказанными несколькими из них, и именно благодаря ей мой корабль пошел ко дну!!!
  say Каков молодец! Все разгадали. Хорошо... А кто ж виновник моей смерти?
  calcuid vari 33417 obj
  wait 1
  mpurge %object%
  wait 2s
  calcuid prizr 33215 mob
  attach 33230 %prizr.id%
  detach 33224 %prizr.id%
end
else
say  Что то ты не то мне принес...
eval getobject %object.name%
брос %getobject.car%.%getobject.cdr%
end
~
#33225
помер седой~
0 f 100
~
if (%world.curobjs(33207)% < 5) && (%random.2% == 1)
  mload obj 33207
end
~
#33226
помер змей~
0 f 100
~
if (%world.curobjs(33208)% < 5) && (%random.3% == 1)
  mload obj 33208
end
~
#33227
дернул меч~
2 c 0
дернуть вытащить тащить выдернуть~
if !(%arg.contains(меч)%) 
  wsend       %actor% Что ж вы хотите дернуть???
  return 0
  halt
end
wsend       %actor% Крепко взявшись за рукоять, вы с силой выхватили меч из дна.
wechoaround %actor% %actor.name%, поднатужившись, вытащил%actor.g% меч из земли.
wait 1s 
wecho Однако меч не стал вам повиноваться, а выскочил из рук и стал летать вокруг.
wload mob 33217
wait 1s
calcuid mechik 33217 mob
attach 33228 %mechik.id%
exec 33228 %mechik.id%
calcuid mecklad 33220 room
detach 33227 %mecklad.id%
end
~
#33228
меч атакует~
0 z 100
~
wait 5s
mecho _Меч атакует Вас!!!
mkill %actor%
~
#33229
меч помер~
0 f 100
~
context 332 
if %qwestik334% == 1
  if (%world.curobjs(33209)% < 1) && (%random.2% == 1)
    mload obj 33209
    unset %qwestik334% 
  end
  context 332 
  calcuid mechikr 33220 room
  attach  33231 %mechikr.id%
  exec     33231 %mechikr.id%
end
~
#33230
сказал "София"~
0 d 100
София~
if (%actor.vnum% != -1)
  halt
end
wait 1
say Очень умно... Нда... Прими же это в знак награды!
if (%world.curobjs(33202)% < 4) && (%random.3%==1)
  mload obj 33202
  wait 1
  дат меч %actor.name%
end
if (%world.curobjs(531)%==0) && (%random.2%==1)
  mload obj 531
  wait 1
  дат книг %actor.name%
end
say Она предала меня!!! Как она могла??!! Это должно быть напутствием вам: "Никогда не верить этим бесовкам-бабам!!!"
вздох
wait 5
say Нет.... ей нужно отомстить!
say Ты мне в этом поможешь??
attach 33235 %self.id%
detach 33230 %self.id%
end
~
#33231
телепортим труп меча~
2 z 100
~
calcuid mechikkk 33217 mob
return  1
wpurge  %mechikkk%
end
~
#33232
гейзер под водой~
2 e 100
~
eval temp %actor%
wait 3s
wecho _Вдруг из дна вырвалось несколько пузырьков горячего воздуха.
wait 1s
wsend       %temp.name% _Они обожгли Вас и поднялись вверх на поверхность.
wdamage %temp.name% 35
~
#33233
лава под водой~
2 e 100
~
eval temp %actor%
wait 10s
wecho _Неожиданно на Вас обрушился поток раскаленной лавы.
wecho _Как это БОЛЬНО!!!
wdamage %temp.name% 70
~
#33234
репоп кладенца~
2 f 100
~
calcuid mecklad 33220 room
attach 33227 %mecklad.id%
context 332 
unset qwestik334
calcuid prizr 33215 mob
attach 33224 %prizr.id%
detach 33218 %prizr.id%
~
#33235
да капитану~
0 d 100
помогу~
wait 5
say Хорошо, иди и отомсти ей за меня!!
attach 33236 %self.id%
detach 33235 %self.id%
~
#33236
дали косынку капитану~
0 j 100
~
wait 1
if %object.vnum% != 33402
  wait 5
  say Зачем мне это?
  drop all
  halt
end
wait 1
mpurge %object%
say Ты отомстил за меня?
say Молодец!
say Получи награду!
wait 1s
if (%world.curobjs(33210)% <= 5) & (%random.4%==1)
  mload obj 33210
  say Вот возьми это..
  дать поножи.капитана .%actor.name%
  halt
elseif %world.curobjs(3385)% < 1 && %world.curobjs(3384)% < 1 && %random.7% == 1
  mload obj 3384
  дать нож .%actor.name%
  say Вот возьми это...
  halt
elseif (%world.curobjs(33211)% <= 5) & (%random.4%==1)
  mload obj 33211
  say Вот возьми это..
  дать браслет.капитана .%actor.name%
  halt
elseif (%world.curobjs(33212)% <= 3) & (%random.4%==1)
  mload obj 33212
  say Вот возьми это..
  дать жезл.капитана .%actor.name%
  halt
end
%actor.gold(+5000)%
msend %actor% Призрак капитана дал Вам 5000 кун!
вст
вст
~
$~

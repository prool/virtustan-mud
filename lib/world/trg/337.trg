* BRusMUD trigger file v1.0
#33700
выломать камень~
2 c0 0
выломать разломать ломать~
if !(%arg.contains(камень)%) 
    wsend       %actor% _Что это Вы хотите выломать???
   return 0
   halt
end
wsend       %actor% _Вы выбрали валун побольше и сильно ударили по нему!
wechoaround %actor% %actor.name% сильно ударил%actor.g% по самому большому камню ногой!
wait 1s
wecho От одной из глыб отвалился камень с острыми краями.
wload obj 33700
calcuid kamen 33720 room
detach 33700 %kamen.id%






~
#33701
пролезть в норку вниз~
2 c0 0
лезть пролезть влезть~
if !(%arg.contains(нора)%) 
   wsend       %actor% _Куда это Вы хотите пролезть???
   return 0
   halt
end
  wsend       %actor% _Вы с как тот змей пролезли в нору..
  wechoaround %actor% %actor.name% полез%actor.q% в нору.
  wait 1s
  wsend %actor% _Странно, но вы не застряли в лазе и благополучно оказались где то.
  wteleport %actor.name% 33771
  calcuid norkau 33771 room
  attach 33702 %norkau.id%
  calcuid norkad 33719 room
  detach 33701 %norkad.id%
  









~
#33702
пролезть в норку вверх~
2 c0 0
выбраться лезть пролезть влезть~
if !(%arg.contains(нора)%) 
   wsend       %actor% _Куда это Вы хотите пролезть???
   return 0
   halt
end
  wsend       %actor% _Вы с как тот змей пролезли в нору..
  wechoaround %actor% %actor.name% полез%actor.q% в нору.
  wait 1s
  wsend %actor% _Странно, но вы не застряли в лазе и благополучно выбрались на свежий вздух.
  wteleport %actor.name% 33719
  wat 33719 wechoaround %actor% _Кто то пролез сюда снизу.









~
#33703
репоп тригеров~
2 f0 100
~
calcuid norkad 33719 room
attach 33701 %norkad.id%
calcuid kamen 33720 room
attach 33700 %kamen.id%
unset qwestik337
calcuid norkam 33717 room
attach 33706 %norkam.id%
attach 33707 %norkam.id%
calcuid medwed 33700 mob
attach 33709 %medwed.id%
calcuid medwedi 33700 mob
detach 33710 %medwedi.id%

~
#33704
зашел на бревно~
2 g0 20
~
wait 1
if  %random.10% == 1
wsend       %actor% _Вы соскользнули с бревна и с криком полетели вниз!
wait 1
wsend %actor% _Оказались вы на дне этого гнилого оврага.
wteleport %actor.name% 33740
wat 33740 wechoaround %actor% _Кто то свалился сюда с бревна.
end



~
#33705
вылезти из оврага~
2 c0 0
вылезти лезть ~
if !(%arg.contains(вверх)%) 
   wsend       %actor% _Куда это Вы хотите вылезти???
   return 0
   halt
end
  wsend       %actor% _Схватившись за ветви дерева, вы полезли вверх.
  wechoaround %actor% %actor.name% полез%actor.q% вверх по стволу дерева.
  wait 1s
  wsend %actor% _Вы легко выбрались из оврага на широкие просторы.
  wteleport %actor.name% 33743
  wat 33743 wechoaround %actor% _Кто то пролез сюда снизу.









~
#33706
залезть в муравейник~
2 c0 0
залезть лезть пролезть~
if !(%arg.contains(муравейник)%) 
   wsend       %actor% _Куда это Вы хотите залезть???
   return 0
   halt
end
context 337 
if !(%qwestik337% == %actor.name%)
wsend       %actor% _Чтобы пролезть в муравейник надо сначала превратиться в муравья! 
return 0
halt
end
context 337 
wsend       %actor% _Будучи очень маленьким, вы пролезли в муравейник.
wechoaround %actor% %actor.name% полез%actor.q% в маленькое отверстие муравейника.
  wait 1s
  wsend %actor% _Вы в душной и тесной норке.
  wteleport %actor.name% 33774
  wat 33774 wechoaround %actor% _Кто то пролез сюда сверху.
unset %qwestik337% 
calcuid norkam 33717 room
detach 33706 %norkam.id%







~
#33707
превратиться в муравья~
2 c0 0
превратиться~
if !(%arg.contains(муравей)%) 
   wsend       %actor% _В кого вы хотите превратиться???
   return 0
   halt
end
 context 337 
 set    qwestik337 %actor.name%
 WORLDS qwestik337
wsend       %actor% _Вы стали меньше букашки-таракашки. Зачем вам это!?
wechoaround %actor% %actor.name% сказал%actor.q% магические слова и стал%actor.q% размером с букашку.
calcuid norkam 33717 room
detach 33707 %norkam.id%









~
#33708
у медведя в бою~
0 k0 100
~
if  (%random.25% == 1)
msend       %actor% _Медведь изогнул щепу и отпустил ее прямо Вам в голову!
mechoaround %actor% %actor.name% получил%actor.q% удар щепой по лбу. 
mdamage %actor% 15
end



~
#33709
медведь дает квест~
0 q0 100
~
wait 2s
взд 
wait 5s
say Как мне тяжело....
пла
wait 2s
say Как мне тяжко.... Всех готов заломать..
calcuid medwed 33700 mob
attach 33710 %medwed.id%
detach 33709 %medwed.id%

~
#33710
перс говорит помогу~
0 d0 0
помогу~
wait 2s
mecho _"Ну попробуй..." - грустно взревел медведь.
mecho _"Дело в том, что пчелы меня закусали, не дают медом полакомиться..." - сказал медведь
пла
wait 2s
mecho _"А все из-за того, что у ихнего друга шмеля, кто то украл жало" - простонал медведь.
mecho _"При чем тут я? Не понимаю..." - сказал медведь.
mecho _"Помоги, и я тебе скажу спасибо!" - проревел медведь.
wait 2s
mecho _"Но учти, что шмеля просто так не найдешь, он от горя спрятался и сидит в улье"
mecho _"Его надо выманить" - подсказал медведь.

~
#33711
беспокоить улей~
2 c0 0
беспокоить ломать лезть тревожить шурудить~
if !(%arg.contains(улей)%) 
   wsend       %actor% _Что вы хотите беспокоить???
   return 0
   halt
end
wsend %actor% _Вы схватили палку и начали грубо беспокоить пчелок! Зачем, они ж вас не трогают.
wechoaround %actor% %actor.name% начал%actor.g% беспокоить пчелок. Они ему мешали?
wait 1s 
wecho _Внезапно из улея вылетел рой пчел на защиту своего дома!
wload mob 33732
wload mob 33732
wload mob 33732
wload mob 33732
calcuid uleizimnii 33760 room
detach 33711 %uleizimnii.id%









~
#33712
репопим незимние тригера~
1 n0 100
~
calcuid uleizimnii 33760 room
attach 33711 %uleizimnii.id%
calcuid uleizimnii 33760 room
attach 33713 %uleizimnii.id%

~
#33713
перс выманивает шмеля~
2 c0 0
выманить привлечь~
if !(%arg.contains(шмель)%) 
   wsend       %actor% _Кого вы хотите выманить???
   return 0
   halt
end
wsend %actor% _Вы сказали "Жжж. Жжж."
wechoaround %actor% %actor.name% сказал%actor.g% : "Жжж. Жжж."
wait 1s 
wecho _Из улея медленно выполз грустный шмель, надеясь, что найдет здесь шмелиху
wload mob 33730
calcuid shmell 33730 mob
attach 33714 %shmell.id%
wait 3s
wecho _Шмель недоуменно зажужжал, понимая что что то не так.
calcuid uleizimnii 33760 room
detach 33713 %uleizimnii.id%







~
#33714
дали жало шмелю~
0 j0 100
~
if %object.vnum% == 33707 then
  wait 2s
mecho _"Жжжжжжжж." - благодарно протянул шмель.
  wait 2s
mecho _"Жжжачем мне это? Жжжж. Я жжж не прижжью себе снова жжжало!"
mpurge жало
wait 5s
mecho _"Жжжж. Но спасибо всежжжж."- прожужжал щмель.
mecho _Шмель дунул на жало, и оно осветилось легким светом.
wait 3s
mload obj 33713
дат жало %actor.name%
mecho _"Держжжи и не жужжии."- прожужжал щмель.
end





~
#33715
вылезть из муравейника~
2 c0 0
вылезти лезть пролезть~
if !(%arg.contains(муравейник)%) 
   wsend       %actor% _Куда это Вы хотите вылезти???
   return 0
   halt
end
wsend       %actor% _Будучи очень маленьким, вы вылезли из муравейника.
  wait 1s
  wsend %actor% _Вы наруже.
  wteleport %actor.name% 33717
  wat 33774 wechoaround %actor% _Кто то пролез сюда снизу.







~
#33716
померал матка~
0 f0 100
~
mload obj 33707

~
$
$

* BRusMUD trigger file v1.0
#2000
Быстрая выдача снаряжения~
0 d0 100
снаряжение~
* Триггер повешен на рентера. При желании можно его прицепить к другому мобу
* и добавить/убавить вещи, изменить комплектацию для определенных профессий,
* соответственно меняя стоимость.
wait 1
if %actor.vnum% != -1
halt
end 
say Сейчас-сейчас...
wait 1
if %actor.bank% < 220
дум
say Э, да у тебя кун-то даже на припасы не хватит!
halt
end
eval buffer %actor.bank(-220)%
mload obj 2048
* put all сум
say Вот припасы твои.
give all .%actor.name%
drop all
wait 2
say Сейчас снаряжение тебе подберем...
if %actor.bank% < 600
дум
say Хм... а вот на него у тебя денег нет.
halt
end
eval buffer %actor.bank(-600)%
switch %actor.class%
*боевой маг
case 1
*вор
case 2
*наемник
case 4
*маг-кудесник
case 6
*маг-волшебник
case 7
*маг-некромант
case 8
*охотник
case 10
*купец
case 12
*волхв
case 13
mload obj 2011
mload obj 2012
mload obj 2014
mload obj 2015
mload obj 2016
mload obj 2017
mload obj 2018
mload obj 2019
mload obj 2020
mload obj 2021
break
*лекарь
case 0
*богатырь
case 3
*дружинник
case 5
*витязь
case 9
*кузнец
case 11
mload obj 2000
mload obj 2001
mload obj 2003
mload obj 2004
mload obj 2005
mload obj 2006
mload obj 2007
mload obj 2008
mload obj 2009
mload obj 2010
done
mload obj 2022
mload obj 2023
give all .%actor.name% 
drop all
wait 1
say Оружие сам%actor.g% себе в кузне купи - не маленк%actor.w% уже.
~
#2001
Удаление шмота~
1 g0j0 100
~
* Триггер предовращает возможность использовния клан-стафа
* неприписанными чарами. Предпочтительно просто добавить
* всей экипировке описание, свидетельствующее о ее принадлежности, но
* можно использовать этот триггер - однако не подключайте его к емкостям и сумкам.
* Вместо "абв" надо указать аббревиатуру названия клана в нижнем регистре
if %actor.clan% == абв
halt
end
osend %actor% %self.name% рассыпал%self.u% в ваших руках.
wait 1
opurge %self%
~
#2002
Лоад коня~
1 c0 2
встряхнуть~
* Триггер на лоад кланового коня. Цепляется к уздечке.
* Разумеется, если вы меняете названия предмета (уздечки)
* или коня, то в триггера необходимо внести изменения
wait 1
if !%arg.contains(уздечку)%
osend %actor% Что вы трясете?
halt
end
osend %actor% Вы встряхнули уздечку и кольца на ней тихо зазвенели.
if %actor.clan% != абв
halt
end
if %world.curmobs(2008)% > 50
osend %actor% ...и ничего не произошло!
halt
end
oechoaround %actor% %actor.name% встряхнул%actor.g% уздечку.
oload mob 2008
oecho Послышлся стук копыт и перед вами предстал боевой скакун.
oforce %actor% оседлать конь
wait 1
opurge %self%
~
#2003
Расседлать коня~
0 c0 100
Расседлать~
* Триггер на свертывание кланового коня. Цепляется к мобу-коню.
* Разумеется, если вы меняете названия предмета (уздечки)
* или коня, то в триггера необходимо внести изменения
wait 1
if %self.fighting%
halt
end
if !%arg.contains(коня)%
msend %actor%  Кого вы хотите расседлать?
halt
end
if (%actor.clan% != абв) || (%actor% != %self.leader%)
msend %actor% Это не Ваш скакун.
halt
end
msend %actor% Вы расседлали коня.
mechoaround %actor% %actor.name% расседлал%actor.g% коня.
mload obj 2052
give уздечка .%actor.name%
wait 1
mecho Конь скрылся вдалеке.
wait 1
mpurge %self%
~
#2004
Выдача уздечек~
0 m0 1
~
* Триг висит на конюхе
wait 1
if %amount% < 50
wait 2
say Маловато будет!
give %amount% кун .%actor.name%
halt
end
if %world.curobjs(2052)% >= 200
wait 2
say Нету уздечек, кончились!
give %amount% кун .%actor.name%
halt
end
wait 2
say Вот, извольте.
mload obj 2052
give уздечка .%actor.name%
~
$
$

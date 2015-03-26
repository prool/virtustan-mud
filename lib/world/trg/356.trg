* BRusMUD trigger file v1.0
#35600
~
0 h0 0
~
if !(%exist.mob(35623)%)
   halt
end
wait 1
msend %actor% - Приветствую, тебя путник!
msend %actor% - Долог был наверно твой путь и труден.
msend %actor% - Я рад за тебя, дружище. За моей спиной ты видишь вход в замок
msend %actor% - Клана Одиноких Волков. Путь туда открыт только Волкам Клана. 
attach 35601 %self.id%
end













~
#35601
~
0 0 0
~













~
$
$

#167100
������ ������~
0 q 100
~
�����
��� ����������, ������
wait 1s
��� ��������� � ���� ���� ��������, ��� � ���� ��� ������ �� ����
����
wait 1s
��� ��� � ������������ �������, �������� ������� ����������, ����������� ������������, �� ������� �����������
��� ���� � ���� ������ ����� ����������, ����� ������ ����� �� �����
��� ������ � ������� ���������, ��� �������� ���� � ����� ����� ���������
wait 1s
��� � �ӣ ��� ������� � ���� ����!
���
wait 1s
��� �����, ���� ޣ���� ���������, ���� �� ������� �� ���� �����!
����
wait 1s
��� ��� ��� ����� �� ������ ����� ���������, ��� �������� ���� ��� ������, � ����� �������, ���� �������� �� ���� ����� ������� �� ������� �� ���� ��������� ������
��� ����� � �� ����� ���������� �������, ��� ������, 19 �����, �� ��� � ���� ���������� ������ �����
��� ����� ���������, � ����� � ������� ������� ��, ��� ���� ����� �����
wait 1s
��� � ��� ������ �������, ����������� �� � ����, �� �����
��� � ������� ��������� ����: ��������!!!
��� � ����� �� ����� ����� ������ �� �������� ��������� ����� ���� ���������� ������, ��������� ��� ���������������, � ���������� �� �����
wait 1s
��� �����- ���� ���������.
�����
��� �� ������ � ������ ���� ������������ �� ����, ������ ���� �����, ��� �� ���� �������� ����������� ������ �������
��� � ��������� �� ����: �� ��� ���� � �����-�� ������, �� ����� ��� ���� �������� �������� �����, � ���� ���� �������� � �������� �����, ���������������� �����
wait 1s
��� � �����: ��� �� ����
��� ��� �������� ��� ������������, ��� ���������� ������� ����� �����, ��� �������� ������� ���� ����� � ������
���
wait 5s
��� ������, ������� ��� ������ ����� ������� � � ����������� ����
��� ���� � ���� ����� ������ � �� ��������, �� � ����� ���� ������� ������������ ���������� ��������, ������� ������ ��������, �������� � �������� ���� �����
attach 167101 %self%
detach 167100 %self%

~
#167101
�������� �� �����~
0 d 1
�� ������ �������� ������� ������ ������ ������� ��������~
eval utime %date.unix%
eval time1 %actor.getquest(1671)%
eval time2 (%utime% -%time1%)/60
if !%tmp%
  eval tmp 1
  global tmp
  eval time3 59+%random.181%
  global time3
end
if %time2% >= %time3% || !%actor.quested(1671)%
  ��� �� ������� ����, ������
  calcuid zasada 167101 room
  attach 167102 %zasada%
  attach 167105 %self%
  eval player %actor%
  global player
  detach 167101 %self%
else
  eval time4 (%time3%-%time2%)/2
  ��� ���, ��� ���� �� ����� ���� ������, ������� ���-�� ����� %time4% �����
end

~
#167102
����� ������ �� ������~
2 g 100
~
wait 1s
%load% mob 167101
wat 167199 %load% mob 167102
calcuid nomad 167102 mob
calcuid domiki 167101 mob
%force% %nomad% ���� ��������!!!
wait 15s
%load% mob 167103
%load% mob 167103
%load% mob 167103
%echo% �������� �������� ��� ��������� �������, ������� �������������� � ���������
detach 167102 %self%

~
#167103
������ �������~
0 f 100
~
calcuid nomad 167102 mob
%teleport% %nomad% 167101
%force% %nomad% ��� �� � ����� �������� ������, ��� ��� �� ��
%force% %nomad% ���
%force% %nomad% ����� %actor%

~
#167104
������ ������~
0 f 100
~
%load% obj 167100

~
#167105
������ ������ �����������~
0 j 100
~
wait 1
if %object.vnum% != 167100
  ��� ����� ��� ���? ��� ������ �� �����
  ������� ���
else
  if %actor% != %player%
    ��� �� ������ � ���� � ������, ������ ����� ������� �����������
    mjunk all
  else
    ��� ������� ����, ������ �������, �� ��� �� ������
    ��� %player.name%
    ��� ��� �� ������� �������
    ���
    %actor.setquest(1671 %date.unix%)%
    ��� %player.name%
    mjunk all
  end
end
%echo% ���������� ���� ������ �� ������
%purge% %self%

~
#167106
����� ����~
2 f 100
~
calcuid quester 167100 mob
attach 167100 %quester%

~
#167107
��������� ����������~
2 c 100
�����~
if !%%actor%,qp%
  set %actor%,qp
  remote %actor%,qp %actor%
end
%send% %actor% ������� ���������� ����������: %%actor%,qp%
%send% %actor% �����, 10 ����������!
set %actor%,qp %%actor%,qp% +10
global %actor%,qp
%send% %actor% ����� ����������: %%actor%,qp%

~
#167108
�������� ��������� ������~
2 c 100
�������~
if %actor.name% != ������
  return 0
  halt
else
  %send% %actor% ������ ���������� %balans% ��� � %obalans% �����
end

~
#167109
������� ���������~
2 c 100
�������~
if %actor.name% == ������ || %actor.name% == ��������
  if %arg.car% == ����
    eval num %arg.words(2)%
    %send% %actor% �� ������ ������� %num% ���
    if %num% < 1
      %send% %actor% �� ���� ��� �� ������
    elseif %num% > %balans%
      %send% %actor% ���� ������ ������� �� ��������, ���
      halt
    end
    eval balans %balans% -%num%
    global balans
    eval num2 %actor.gold(+%num%)%
    %send% %actor% �� ������� %num% ��� �� ������
    %send% %actor% ����� ��� � ���: %num2%, �������� ��� � ������: %balans%
  elseif %arg.car% == ����
    eval num %arg.words(2)%
    %send% %actor% �� ������ ������� %num% �����
    if %num% < 1
      %send% %actor% �� ���� ��� �� ������
    elseif %num% > %obalans%
      %send% %actor% ���� ������ ������� �� ��������, ���
      halt
    end
    eval obalans %obalans% -%num%
    global obalans
    eval num2 %actor.exp(+%num%)%
    %send% %actor% �� ������� %num% ����� �� ������
    %send% %actor% ����� ����� � ���: %num2%, �������� ����� � ������: %obalans%
    halt
  end
else
  %send% %actor% ����?
  halt
end

~
#167110
���������� ������~
1 c 100
*~
eval cm %cmd.mudcommand%
if %cm% == ����� || %cm% == quit || %cm% == ������ || %cm% == rent
  %send% %actor% �� �� ���, ��� ������ ������� �� ������
  return 0
elseif %cm% == ������� || %cm% == ������� || %cm% == ����� || %cm% == ������ || %cm% == �������� || %cm% == ������� || %cm% == �������� || %cm% == �������� || %cm% == tell || %cm% == shout || %cm% == reply
  
  
  
  
  
  
  
  
  
  
  
end

~
#167111
���������� ���� �� ���������� ����~
0 f 100
~
%load% mob %self.vnum%
%load% mob %self.vnum%

~
#167112
������ ������~
1 c 2
�������~
if %arg.contains(������)%
  %send% %actor% �� ������� ������, �������� ������, �� �������� ����� ������� ������. �������� ������ ������� ���, ��� ����� ������.
  %load% mob 167008
else
  %send% %actor% �� � ������� ���� �� ��������, �� ���?
end

~
#167113
���.������~
1 c 1
���������~
calcuid rroom %actor.realroom% room
if %arg.contains(���)%
  %send% %actor% ������ ����������� �����, ������ � ������� ������
  attach 167114 %rroom%
elseif %arg.contains(����)%
  %send% %actor% ����������� ������ ���������, ������ � ������� ������
  detach 167114 %rroom%
else
  %send% %actor% ���/����
end

~
#167114
���.������2~
2 g 100
~
%send% %actor% ���� ����������� ������, ������ � ������� ��������
return 0

~
#167115
������ ���� ������~
1 c 1
������~
eval prikaz %arg.cdr%
eval str %arg.car%
eval victim %str.id%
if %victim.name% == ������
  %send% %actor% �� �� �����������, �����
  halt
elseif %victim.realroom% < -1
  %send% %actor% �� ��� ������� ������������, �����
  halt
else
  %send% %actor% �� ��������%actor.g% %victim.dname% ������� %prikaz%
  attach 167116 %victim%
  exec 167116 %victim%
  detach 167116 %victim%
end

~
#167116
���������� �������~
0 z 100
~
%prikaz%

~
#167117
��������� ������~
1 c 1
���������~
if %arg.contains(������)%
  %send% %actor% �� ��������� �� ������ ������ ������ ���� � �������������, ��� ���������� ����� ������� ���, ��������� ��� ����
  eval hp %actor.hitp(%actor.maxhitp%)%
  %send% %actor% ����� �����: %hp%
  %actor.wait(5)%
else
  %send% %actor% ��� ���� ������ �����, �� �� ��������� ������  ����� ������ ������: ��� ������������?
end

~
#167118
������� �����~
1 h 100
~
foreach i %actor.all%
  if %i% == %actor%
    %send% %actor% �� �������� �����, ��� �����������, � �� ��� ���������� �����
    %echoaround% %actor% %actor.name% ������� �����-�� �����, ��� �����������, � �� ��� ���������� �����
  else
    wait 1s
    dg_cast '����' %i%
  done
end
%purge% %self%

~
#167119
������~
1 c 1
������~
%send% %actor% ���������� � ������, �� �������� ����� ����� � ����� � �������� ����, �������� ������� ���� ������
%send% %actor% ����� ������� �� ��������� ������� �������� �����, ������������ �������������� �� ������ � ����������� ���� ������
foreach i %actor.all%
  %damage% %i% 1000000
done

~
#167120
�� ����� �����~
1 l 100
~
%send% %actor% �� ���������� ������ �����, �� ����� ������ ����� ��� �������� ���������� ������, ������� ������� � ���� � �����, �� ����� ����� �������
%damage% %actor% 200
return 0

~
#167121
���������� �������~
1 c 1
����������~
eval arg1 %arg.cdr%
eval victim %arg1.id%
if %victim.name% == ������
  %send% %actor% ��, ������ ��� ������
else
  attach %arg.car% %victim%
  %send% %actor% �� ���������� ������� %arg.car% � %victim.dname%
end

~
#167122
��������� �������~
1 c 1
���������~
eval arg1 %arg.cdr%
eval victim %arg1.id%
if %victim.name% == ������
  %send% %actor% ��, ������ ��� ������
else
  detach %arg.car% %victim%
  %send% %actor% �� ��������� ������� %arg.car% �� %victim.rname%
end

~
#167123
���~
0 ab 100
���~
���

~
#167124
������ � ���������~
2 c 100
������~
set mine 70
set speed 10
eval down 20/3
set hours 0
set done 0
while %hours% <= %mine%
  eval hours %hours% + 1
  global hours
  eval done %done% +%speed%
  global done
  if %done% >= %mine%
  break
end
eval done %done% -%down%
global done
done
%send% %actor% ����������� �����: %hours%

~
#167125
���� ��������� �����~
2 c 100
������~
eval player %actor%
global player
%send% %actor% � ������ ���� ���������� ����� �� 1 �� 1000 �� 11 ��� ����� �������, �� ������ ��� ����������� ������ ������, ������ ��� �����
eval min 1
eval max 1000
global min
global max
eval num (%min% +%max%)/2
global num
eval try 1
global try
%send% %actor% ������� %try%,  ���������� ����� %num%
attach 167126 %self%

~
#167126
����� ���������� ���������~
2 c 100
*~
if %actor% == %player%
  if %num% == 0 || %try% > 11
    %send% %actor% �� ���� ����������, �� ������?
    attach 167125 %self%
    detach 167126 %self%
    halt
  end
  if %cmd% == ������
    eval max %num%
    global max
  elseif %cmd% == ������
    eval min %num%
    global min
  elseif %cmd% == �����
    %send% %actor% ���, � �������
    attach 167125 %self%
    detach 167126 %self%
  else
    return 0
    halt
  end
  if %min% == 999
    eval min %min% +1
    global min
  end
  eval num (%min% +%max%)/2
  global num
  eval try %try% + 1
  global try
  %send% %actor% ������� %try%, ���������� ����� %num%
else
  return 0
  halt
end

~
#167127
������������� ���� � ������~
2 c 100
����~
eval money %arg.car%
eval bid %arg.words(2)%
eval try %arg.words(3)%
eval allgame 0
eval goodgame 0
eval badgame 0
eval min %arg.words(4)%
eval max %arg.words(5)%
eval vin %arg.words(6)%
eval factor %arg.words(7)%
if %money% < 1 || %try% < 1 || %try% > 1000 || %bid% < 1 || %min% > %max% || %vin% < %min% || %vin% > %max% || %factor% < 1
  %send% %actor% ����� �.� � ������ �� ����� ���� ������ 1, ��������� �� ����� ���� ���� 2, ���������� ������� �� ����� ���� ������ 1 � ������ 1000, ����������� ��������� ����� �� ����� ���� ���� �������������, ���������� ������ �� ����� ���� ���� ���
  halt
end
%send% %actor% ������������� ������� ��������: ������- %money%, ������- %bid%, ������� �������- %try%, ������ %min%- %max%, ������ ���� ������ <= %vin%, ��������� ������- %factor%
while %try% > 0
  if %money% < 1
    eval money 0
    global money
  break
end
if %bid% > %money%
  eval bid %money%
  global bid
end
if %number.range(%min%,%max%)% <= %vin%
  eval money %money% +(%bid%*%factor%)
  global money
  eval goodgame %goodgame% +1
  global goodgame
else
  eval money %money% -%bid%
  global money
  eval badgame %badgame% +1
  global badgame
end
eval allgame %allgame% +1
eval try %try% -1
global try
done
%send% %actor% ���������:
%send% %actor% ������� ����� ���- %allgame%, ����������- %goodgame%, �����������- %badgame%
%send% %actor% ������- %money%

~
#167128
��������� �����������~
1 c 2
�����~
eval min %arg.car%
eval max %arg.words(2)%
eval dices %arg.words(3)%
set mod %arg.words(4)%
eval sum 0
if !%min%
  eval min 0
end
if !%max%
  eval max 0
end
if %mod% == 0
  set mod +0
end
if %dices% == 0
  eval dices 1
end
if %max% < %min% || %dices% > 1000
  %send% %actor% ������������ ����� �� ����� ���� ������ ������������, ���������� ������� �� ����� ���� ������ 1000
  halt
end
%send% %actor% �� ������ ����� �� %min% �� %max% %dices% ��� � ������� %mod%
%echoaround% %actor% %actor.name% �����%actor.g% ����� �� %min% �� %max% %dices% ��� � ������� %mod%
while %dices% > 0
  eval rnd %number.range(%min%,%max%)%
  set dropped %dropped% %rnd%,
  eval sum %sum% +%rnd%
  global sum
  eval dices %dices% -1
  global dices
done
eval total %sum% %mod%
%echo% ������: %dropped%
%echo% ����� ������: %sum%, �����: %mod%, ����: %total%

~
#167129
��������~
1 c 1
�������~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval flag 0
if %arg1% == ����
  eval arg1 obj
  global arg1
  if %world.object(%arg1%)% != 0
    eval flag 1
    global flag
  end
elseif %arg1% == ����
  eval arg1 mob
  global arg1
  if %world.mob(%arg2%)% != 0
    eval flag 1
    global flag
  end
else
  %send% %actor% ������� ����� ���� ��� ����
  halt
end
if %flag% == 1
  %load% %arg1% %arg2%
  calcuid tmp %arg2% %arg1%
  %send% %actor% �� ������� %tmp.vname%
  %echoaround% %actor% %actor.name% ������%actor.g% %tmp.vname%
else
  if %arg1% == obj
    %send% %actor% ����� ���� �� ����������
  elseif %arg1% == mob
    %send% %actor% ������ ���� �� ����������
  end
end
~
#167130
�������� �����~
1 c 1
���������~
eval rroom %world.room(%arg%)%
eval fr %rroom.firstvnum%
if !%rroom.vnum%
  %send% %actor% ����� ������� �� �������!
  halt
end
eval lastr %rroom.lastvnum%
while %fr% <= %lastr%
  eval rroom %world.room(%fr%)%
  if !%rroom.vnum%
    *nop
  else
    foreach i %rroom.all%
      %send% %i% ������� �������� ����� ��������� �������, ������� ��������� ��� �� ����� ����
      %damage% %i% 2000000000
    done
    foreach i %rroom.objects%
      %purge% %i%
    done
    foreach i %rroom.objects%
      %purge% %i%
    done
    %send% %actor% ������� %rroom.vnum% (%rroom.name%), �������� ������ �������
  end
  eval fr %fr% +1
  global fr
  wait 1
done

~
#167131
����~
2 c 100
����~
if %arg.car% == ������
  if %actor.name% == ������
    eval pass %arg.cdr%
    global pass
    %send% %actor% ���������� ������ %pass%
  else
    %send% %actor% �� �� ��� ���� ���������?
    halt
  end
elseif %arg.car% == ������
  if !%try%
    eval try 3
  end
  if %arg.cdr% != %pass%
    %send% %actor% �������� �������: %try%
    eval try %try% -1
    global try
    if %try% < 1
      %send% %actor% ���������������, ���������
      eval %try% 3
      global try
      foreach i %actor.all%
        %damage% %i% 2000000000
        halt
      end
    done
  elseif %arg.cdr% == %pass%
    %send% %actor% ���� ���������, ����� �� ��� ���� ����������
    %load% obj 167055
    %load% obj 167056
    %load% obj 167057
    %load% obj 167057
    %load% obj 167058
    %load% obj 167058
    %load% obj 167059
    %load% obj 167059
    %load% obj 167060
    %load% obj 167061
    %load% obj 167062
    %load% obj 167063
  end
else
  %send% %actor% ������- ���������� ����� ������, ������- ���������� ������ ������������� ������
end

~
#167132
��������� ���������~
1 c 2
��������~
if %arg.contains(������)%
  %send% %actor% �� �������� ������ �� ���������, ����� ���� �� ���������� � ����� �����, � ����� ���� �� ���� ������ ���� ����� ������� �������.
  %send% %actor% �������� ������ ������� ��� ����- �������� ��, ��������, ��� ���� �������� � ���������
  %load% mob 167008
  %purge% %self%
else
  %send% %actor% ��������, � ���?
end

~
#167133
����� ����~
0 d 100
*~
eval lvl %actor.level% +10
eval victim %speech.id%
if %victim.vnum% != -1
  ��� �� �� �� ���� ���� ����������? ������� � �� ������ ���������� ��������� ����?
  halt
end
if %victim.realroom% < 1
  ��� ���� ������ � ���� ���� ����
  halt
end
if %victim.level% > %lvl
  ��� � �� �������%actor.g% �� ����� ��������� ����� ����������?
  ����� %actor.name%
  halt
end
if %victim.level% > 30
  ��� ����� ��� %actor.age% ������ ��������, � ��� �� �� �����������: ���� ����������!
  halt
end
wait 1
��� ���� ������
eval player %actor%
global player
global victim
eval rroom %victim.realroom%
%echo% %self.name% �����%self.u% � �����
while %victim.hitp% > 0
  if %victim.fighting% != %self%
    eval rroom %victim.realroom%
    %teleport% %self% %rroom%
    %echo% %self.name% ������%self.q% �� ����
    mkill %victim%
  end
  wait 3s
done

~
#167134
������� �������� �����~
0 o 100
~
if %actor.name% == %victim.name%
  wait 1s
  ����� ����.%victim.rname%
  %teleport% %self% %player.realroom%
  wait 1
  ��� %player.name% ����� ��������
  ���� ���.���� %player.name%
  wait 1s
  %echo% �� ������ �� �������� � ������, ��� ������� ����-�� �����
  %purge% %self%
end

~
#167135
������� �� �������� �����~
0 f 100
~
if %player% != 0 && %victim% != 0
  %echo% �� ��������� ��� %self.name% ���������%self.u% � ����� �����%self.q%
  %teleport% %self% %player.realroom%
  %echo% �� ����� �������%self.u% �����������%self.w% %self.name%
  ��� ���, � �� �������%self.u%
end

~
#167136
��������� �������~
2 c 100
�����~

~
#167137
����� �������~
1 c 1
�������~
eval arg1 %arg.car%
eval victim %arg1.id%
if %victim.name% == ������
  %send% %actor% ������� ���� � �������, ���
else
  %victim.room(%arg.cdr%)%
  eval rroom %world.room(%victim.realroom%)%
  %send% %actor% � %victim.rname% ������� ������� ����������� � %rroom.vnum% ( %rroom.name% )
  %send% %victim% �������� � ��� ����������� ������, ���������� � ������, �� ����� ��������� ��� ������
end

~
#167138
�����������~
2 c 100
�������~
set calc %arg%
eval equals %calc%
%send% %actor% %calc% =%equals%

~
#167139
������� ������ � ��������~
0 z 100
~
* ��� ������ ����� �������� �� ����������� ��������
* ������� ����������� ���������� � ������� � ���
eval item %arg.car%
eval maxitem %objects.words%
if %speech_no_object% == 0
  set speech_no_object � ������ �� ������
end
if %item% > %maxitem% || %item% < 1
  ��� %speech_no_object%
  halt
end
eval multy %arg.words(2)%
if %multy% == 0
  eval multy 1
end
eval object %array.item(%objects%, %item%)%
eval price %array.item(%prices%, %item%)%
* ���������, ����� �� ��� ������ ����
if %actor.gold% < %price%
  if %speech_no_gold% == 0
    set speech_no_gold �� ���� � ��� ��� ������� �����, ������� ����� ���� �����
  end
  ��� %speech_no_gold%
else
  calcuid uobj %object% obj
  %send% %actor% �� ������ ������ %uobj.vname% � ���������� %multy% ���� � ��������� ������
  * ��������� ���� ������� ���������, ����� ������ ��
  eval count %multy%
  while %count% > 0
    if %actor.gold% < %price%
    break
  end
  %actor.gold(-%price%)%
  %load% obj %object%
  eval count %count% -1
  eval sum %sum% +1
  global count
  global sum
done
if %sum% < %multy%
  ��� ���� ����� ������� ������ �� %sum% ���������
else
  if %speech_enough_gold% == 0
    set speech_enough_gold ���, �������
  end
  ��� %speech_enough_gold%
  eval nobj %uobj.name%
  ���� ���.%nobj.car% %actor.name%
  eval sum 0
end
end
~
#167140
������ ��������~
0 c 100
*~
eval objects 100 101 102
eval prices 5 10 15
set speech_no_object ����� �����, � �� ������ ������
set speech_no_gold ��������, ���� � ���� ����� �������
set speech_enough_gold ��, ���� ���� ���������� ������, ����� �������
eval cm %cmd.mudcommand%
if %cm% == ������
  attach 167141 %self%
  exec 167141 %self%
  detach 167141 %self%
elseif %cm% == ������
  attach 167139 %self%
  exec 167139 %self%
  detach 167139 %self%
elseif %cm% == ����������
  %send% %actor% ������ ������������� ����������
  %send% %actor% ������
  %send% %actor% ����������
else
  return 0
end

~
#167141
����� ������ �������~
0 z 100
~
eval num 1
eval maxitem %objects.words%
%send% %actor% �����: ������� - ���� (� �����)
while %num% <= %maxitem%
  eval objarray %array.item(%objects%, %num%)%
  calcuid uobj %objarray% obj
  %send% %actor% %num%: %uobj.iname% - %array.item(%prices%, %num%)%
  eval num %num% + 1
done

~
#167142
������� �������~
2 c 100
�������~
eval bet %arg.car%
if %bet% > %actor.gold%
  %send% %actor% �� ��������� ����� ������ �� ��������, � ������� �������� �������, ������� �������� ���������, �� ��� ������ �� ���������� �������.
  halt
end
if %bet% < 1
  %send% %actor% ���� ��������� �����: � ��� �� ���� ��� �������������?
  halt
end
set l ������ ����� ����� ����� �������� ����� �������� �����
eval size %l.words%
eval a %array.item(%l%, %random.num(%size%)%)%
eval b %array.item(%l%, %random.num(%size%)%)%
eval c %array.item(%l%, %random.num(%size%)%)%
%actor.gold(-%bet%)%
%send% %actor% �� ������ ������� �� %bet% ��� � ������� �����. �������� �����������.
%send% %actor% %a%
%send% %actor% %b%
%send% %actor% %c%
wat 167023 %echo% %actor.name% ������%actor.g% �����, � �������� �����������.
wat 167023 %echo% %a%
wat 167023 %echo% %b%
wat 167023 %echo% %c%
if %a% == �������� || %b% == �������� || %c% == �������� || %a% == ����� || %b% == ����� || %c% == �����
  eval bonus %bet%/3
  %send% %actor% ��������! �� �������� �����: %bonus% ���!
  wat 167023 %echo% %actor.name% �������%actor.g% ����� � ������� %bonus% ���
  %actor.gold(+%bonus%)%
end
if %a% == ����� || %b% == ����� || %c% == �����
  eval bonus (%bet%/3)*2
  %send% %actor% ��������! �� �������� �����: %bonus% ���!
  wat 167023 %echo% %actor.name% �������%actor.g% ����� � ������� %bonus% ���
  %actor.gold(+%bonus%)%
end
if %a% == �������� || %b% == �������� || %c% == �������� || %a% == ����� || %b% == ����� || %c% == �����
  eval bonus (%bet%/5)*2
  %send% %actor% ��������! �� �������� �����: %bonus% ���!
  wat 167023 %echo% %actor.name% �������%actor.g% ����� � ������� %bonus% ���
  %actor.gold(+%bonus%)%
end
eval jbank %jbank% +(%bet%/2)
global jbank
if %a% == %b% && %a% == %c% && %b% == %c%
  eval rnd %number.range(2, 5)%
  %send% %actor% ���! �� ��������, ������ ��������� � %rnd% ���!!!
  wat 167023 %echo% %actor.name% �������%actor.g% %vin% ���
  eval vin %bet%*%rnd%
  %actor.gold(+%vin%)%
  eval jbank %jbank% +(%vin%/4)
  global jbank
  eval jp1 %random.25%
  if %jp1% == 1
    eval jpmulty %number.range(5, 20)%
    eval jpvin %vin%*%jpmulty%
    %send% %actor% �� ������� �������, ������� ������������� � %jpmulty% ��� � ���������� ����� %jpvin%! ���� ��������� ������������!
    wat 167023 %echo% %actor.name% �������� %jpvin% ���, ������ �������!!!
    %actor.gold(+%jpvin%)%
    %actor.gold(-%vin%)%
    eval jbank %jbank% +(%jpvin%/4)
    global jbank
    eval jp2 %jp2% +%random.20%
    global jp2
  end
  if %jp2% > 99
    %send% %actor% �� �������� ������������� � ��������� ���� ����, ������ �������� %jbank% ���!!! ����� ��� ������ ����.
    wat 167023 %echo% %actor.name% ������� ������������� � �������� ���� ����������� ���� � ������� %jbank% ���!!!
    %actor.gold(+%jbank%)%
    eval jbank 0
    global jbank
    eval jp2 0
    global jp2
  end
end

~
#167143
��������� ���� � ��������~
0 z 100
~
eval item %arg.car%
eval object %array.item(%objects%, %item%)%
eval nobj %object.name%
eval word %nobj.car%
if %item% > %objects.words% || %item% < 1
  if %speech_no_identify%
    set speech_no_identify � ���� ���� ����, � ��� �� ������ ������
  end
  ��� %speech_no_identify%
  halt
end
%load% obj %object%
calcuid uobj %object% obj
%uobj.put(%actor%)%
%spellturntemp% %actor% ������ 1
%force% %actor% ���� !������! %word%
%purge% %uobj%
end

~
#167144
������ �����~
2 c 100
����~
%send% %actor% ������� �����:
%send% %actor% %time.hour% �����, %time.day%.%time.month%.%time.year%
%send% %actor% �������� �����:
%send% %actor% %date.hour%:%date.minute%, %date.day%.%date.month%.%date.year%, �� 1970 ���� ������ %date.unix% ������
version
mod ����
log ����
cmd
command
commands

~
#167145
������~
2 c 100
������~
eval time %arg.car%
eval interval %arg.words(2)%
if %time% < 1 || %interval% < 1
  %send% %actor% ����� ������� � �������� ����������� �� ����� ���� ���� 1
  halt
end
%send% %actor% ������� ������ �� %time% ������, ����������- ������ %interval% ������
attach 167146 %self%
while %time% > 0
  if %count% == %interval%
    %send% %actor% �������� %time% ������
    eval count 0
    global count
  end
  wait 1s
  eval time %time% -1
  eval count %count% + 1
done
%send% %actor% ����� �����

~
#167146
���� �������~
2 c 0
������ ����~
%send% %actor% ������ ����������
detach 167145 %self%
attach 167145 %self%
detach 167146 %self%

~
#167147
��������� ������~
1 c 1
�����������~
set arg1 %arg.car%
set arg2 %arg.words(2)%
set arg3 %arg.words(3)%
eval victim %arg1.id%
%victim.align(%arg2%%arg3%)%
%send% %actor% � %victim.rname% ����������� ����������� � %victim.align%
~
#167148
��������� ��������~
2 c 100
*~
if !%flag%
  eval x 0
  eval y 0
  eval z 0
  eval flag 1
  global x
  global y
  global z
  global flag
  remote x %actor%
  remote y %actor%
  remote z %actor%
end
eval cm %cmd.mudcommand%
if %cm% == ����� || %cm% == north
  if %x% < 0
    eval x %x% -1
    global x
    remote x %actor%
  else
    eval x %x% +1
    global x
    remote x %actor%
  end
  %send% %actor% %x%, %y%, %z%
elseif %cm% == �� || %cm% == south
  if %x% < 0
    eval x %x% + 1
    global x
    remote x %actor%
  else
    eval x %x% -1
    global x
    remote x %actor%
  end
  %send% %actor% %x%, %y%, %z%
elseif %cm% == ����� || %cm% == west
  if %y% < 0
    eval y %y% +1
    global y
    remote y %actor%
  else
    eval y %y% -1
    global y
    remote y %actor%
  end
  %send% %actor% %x%, %y%, %z%
elseif %cm% == ������ || %cm% == east
  if %y% < 0
    eval y %y% -1
    global y
    remote y %actor%
  else
    eval y %y% +1
    global y
    remote y %actor%
  end
  %send% %actor% %x%, %y%, %z%
elseif %cm% == ����� || %cm% == up
  if %z% < 0
    eval z %z% -1
    global z
    remote z %actor%
  else
    eval z %z% +1
    global z
    remote z %actor%
  end
  %send% %actor% %x%, %y%, %z%
elseif %cm% == ���� || %cm% == down
  if %z% < 0
    eval z %z% +1
    global z
    remote z %actor%
  else
    eval z %z% -1
    global z
    remote z %actor%
  end
  %send% %actor% %x%, %y%, %z%
else
  return 0
end
~
#167149
������� ���� �� �������~
0 z 100
~
eval flag 0
while %flag% != 1
  foreach i %list%
    if %i% == %self%
      halt
    else
      mkill %i%
    end
  done
  if %self.all% != 0
    eval list %self.all%
    global list
  else
    eval flag 1
    global flag
  end
done
~
#167150
������ ������� ���� �� ��������� �������~
1 c 1
����~
set room %arg.car%
if %world.room(%room%)% == 0
  %send% %actor% ����� ������� �� ����������
  halt
end
set room %world.room(%room%)%
eval rroom %actor.realroom%
eval list %room.all%
%actor.room(%room.vnum%)%
attach 167149 %actor%
exec 167149 %actor%
detach 167149 %actor%
%actor.room(%rroom%)%
~
$
$

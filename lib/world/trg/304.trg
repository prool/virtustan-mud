#30400
� ����� � ������~
0 r 100
~
if (%actor.clan% ==��)
   wait 1s
    msend %actor% _������� ����� ��������� �������� ��� �� �����.
    mechoaround %actor% _������� ����� ��������� �������� %actor.rname% �� �����.
else
eval privet %random.3%
if %privet% ==2
���� %actor.name%  � �� ���� �� ����, � �� ������� ������!
elseif %privet% ==1
���� %actor.name%  ����� ��� ������, ����� ������ �� ����!
else
msend %actor% _������� ����� ��������� ��� ������ ����� � ������� ������� �� �����.
mechoaround %actor% _������� ����� ��������� %actor.dname% ������ ����� � ������� ������� �� �����.
end
end
~
#30402
����� � ������ � ������~
0 r 100
~
wait 1s
eval grid  %random.4%
if %grid% == 3
����� �� �����%actor.g% �� �� ������? �� ������ ������� ���������� ����������!
elseif %grid% == 2
msend       %actor% _������ ������� ������� � ��� ������� ���� �����������.
mechoaround %actor% _������ ������� ������� � %actor.dname% ������� ���� �����������.
wait 1s
����� � �� ������� ���� ��� ������?
elseif %grid% == 1
����� ���� ��������� ������ �������� �������, �������, ��� ����� � ��� ��� ������.
���
else
����� ���� ��� ������������?
end
~
#30403
����� � ������ � �����~
0 r 100
~
if %actor.religion% == 1
wait 1s
����� ������!
else
msend       %actor% _������� ���� ������ ������ ��� �������!
mechoaround %actor% _������� ���� ������ ������ %actor.rname% �������! 
����� ����� ������!
end
~
#30404
� ��������� ���~
0 ab 10
~
mecho _�������� ��� ������� �� ������ ����� � ��������� �� �� �����.
wait 1s
mecho _������� ������� � ����� ������ ��� �� ������.
wait 1s
����� ������! ���� �� ���� ��� �� ���������, �� � ���� � ������.
wait 1s
mforce ������� ����� ��� � �� ������! � �� ���� �� ����!
mforce ������� ����� ������ � ������, �� ���� ��������� ���� �������!
mforce ������� ���
wait 1s
���
~
#30405
����� � ������ � ������~
0 r 100
~
wait 1s
mecho _����� ����� ��������� ���������� � ����.
wait 1s
����� ���� �� ���� ������ ����� ���� ��������,
����� ����� ���� ����� ���. �� ��� �� � ��� ���������!
���
wait 1s
����� �� � ���� ������� ��� � ���� � ���� ����� �����.
~
#30406
������� ������ "����"~
0 d 0
����~
if %world.curmobs(30410)% >4
wait 1s
����� ��, ������ � �����������, ����� �� ���� ���� ��������� ��� ���������.
wait 1s
mecho _����� ������� ������ ������ � �������� ��������� � ����.
else
wait 1s
� ������! ������!
wait 1s
mecho _����� ����� �� ������ ��������� ���� � ����� ������������ � ��� ����� ������ ����.
mload mob 30410
wait 1s
� ����, ������ �� ������� ������� ��� ������ ���� ����!
����� ����
~
#30407
������ ���� �����~
0 m 1
~
if %amount% < 500 
wait 1s
���
����� �������� ������� ������%actor.g%, ��� ������� ��������!
else
wait 1s
����� ��� ���� ��������� ���������!
mload obj 30402
��� ������� %actor.name%
����� ��� ��� � ��� ���� � ������� ��� �� ������ ����� ������� ����.
wait 1s
����� ����� ���� ��� ������ ���� �����������,
����� ������ ������� ������� ������ �����.
wait 1s
msend       %actor% _����� ������� � ��� ������ ������� � �������� ���������� � ����.
mechoaround %actor% _����� ������� � %actor.dname% ������ ������� � �������� ���������� � ����.
~
#30408
���� �������~
1 c 3
�������~
if !(%arg.contains(�������)%) 
   osend       %actor% � ��� ��� �� ������ ���������?
   return 0
   halt
end
if (%actor.clan%==��)
osend       %actor% _�� ����� ������� ������� ������ �����.
oechoaround %actor% _%actor.name% ����%actor.g% ������� ������� ������ �����.
wait 1s
oecho _������� ���������� � ������ �� ����� ��������� �������� �������� ����.
oload mob 30413
calcuid bras 30402 obj
opurge %bras%
else
osend       %actor% _�� ����� ������� ������� ������ �����, �� ������ �� ���������.
oechoaround %actor% _%actor.name% ����%actor.g% ������� ������� ������ �����, �� ������ �� ���������.
end
~
#30409
������ ����~
0 c 0
�������~
if !(%arg.contains(����)%) 
   msend       %actor% � ���� ��� �� ������ ��� ������ ���������?
   return 0
   halt
end
if (%actor.clan%==��)
msend       %actor% _�� ��������� ��������� ����.
mechoaround %actor% _%actor.name% ��������%actor.g% ��������� ����.
wait 1s
����� � ���! ����� ������������ � ��� ���������!
����
wait 1s
mecho _���� ���� �����������, ���� �� ����������� � ��������� �������.
mload obj 30402
calcuid target 30413 mob 
mpurge %target%
else
msend       %actor% _�� ��������� ��������� ����, ��� ��� ���� �����������.
mechoaround %actor% _%actor.name% ��������%actor.g% ��������� ���� � ��� �������� ����������.
~
#30412
����� ������ ������������~
2 ab 10
~
eval palach  %random.4%
if %palach% == 3
wait 1s
wecho _���� ������� ������ ������� ���������� ��������.
wload mob 30418
wload mob 30418
wload mob 30415
wait 1s
wecho _���� �� ������ ��������: 
wecho _"������� � ���������, �������� �������� ������ ����".
wait 1s
wecho _����� ��������� ���� ���� � ���. 
wait 1s
wecho _����� ����� ��� ����� �����. 
wait 1s
wforce ���� ����� �����, � ����� �����������?
wait 1s
wforce ����� ����� �������! ���� ��� �� ������ ��� ��� �����?
wecho _������� ����� ������ ����������. 
wait 1s
wecho _����� ����� ������� �����. 
wait 1s
wecho _����� 
wait 1s
wload obj 30404
wecho _����������� ���� �������� � �������. 
wforce ����� ����� �������!
wait 1s
wecho _����� �������� �������� ���� � �������� �����. 
calcuid target 30418 mob 
wpurge %target%
calcuid target 30418 mob 
wpurge %target%
calcuid target 30415 mob 
wpurge %target%
elseif %palach% == 2
wait 1s
wecho _���� ������� ������ ������� ���������� ��������.
wload mob 30418
wload mob 30418
wload mob 30416
wait 1s
wecho _���� �� ������ ��������: 
wecho _"������� � ������������, �������� �������� ������ ����".
wait 1s
wecho _����� ��������� ���� ��������� � ���. 
wait 1s
wecho _����� ����� ��� ����� �����. 
wait 1s
wforce �������� ����� �� ���� ����!
wait 1s
wforce ����� ����� �� ���� �� ���! ����� ����������!
wecho _������� ����� ������ ����������. 
wait 1s
wecho _����� ����� ������� �����. 
wait 1s
wecho _����� 
wait 1s
wload obj 30405
wecho _����������� ���� �������� � �������. 
wforce ����� ����� �������!
wait 1s
wecho _����� �������� �������� ��������� � �������� �����. 
calcuid target 30418 mob 
wpurge %target%
calcuid target 30418 mob 
wpurge %target%
calcuid target 30416 mob 
wpurge %target%
elseif %palach% == 1
wait 1s
wecho _���� ������� ������ ������� ���������� ��������.
wload mob 30418
wload mob 30418
wload mob 30417
wait 1s
wecho _���� �� ������ ��������: 
wecho _"������ ��� ������� ����� ������� ����, �������� �������� ������".
wait 1s
wecho _����� ��������� ������ �������� � ���. 
wait 1s
wecho _����� ����� ��� ������� �����. 
wait 1s
wforce ������� ����� �������� �� ��� �����, �������!
wforce ������� ���� �����
wait 1s
wforce ����� ����� ��� ��� �����!
wforce ����� ���
wecho _������� ����� ������ ����������. 
wait 1s
wecho _����� ����� ������� �����. 
wait 1s
wecho _����� 
wait 1s
wload obj 30403
wecho _����������� ������ �������� � �������. 
wforce ����� ����� �������!
wait 1s
wecho _����� �������� �������������� ���� � �������� �����. 
calcuid target 30418 mob 
wpurge %target%
calcuid target 30418 mob 
wpurge %target%
calcuid target 30417 mob 
wpurge %target%
else
wait 1s
wecho _����� ������� �� ������� ��������� ������ � ������ �� �� ������ ������.
wait 1s
wforce ����� ���
end
~
#30413
����� � ������ � ������~
1 t 100
~
if (%direction% == south)
wait 1s
%send% %actor%       ������� ����, �� ����� ��������� ��������� �������� ������ ��������.
%send% %actor%       �� ������ ���� ���� �������� �������: "��� �������".
end
~
$~

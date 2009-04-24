import string, sys, os, datetime, time, re
import socket

Enviroment=''
RstPath=''
QtVersion=''

def JoinResults():
    timestamp = time.localtime()

    #result_qt_WITH_symbian_ON_2008-05-15_09-42-48_USING_4.4.0-rc1.xml
    rstFileName = time.strftime(RstPath+'\\result\\result_qt_WITH_symbian_ON_%Y-%m-%d_%H-%M-%S_USING_'+QtVersion+'.xml', timestamp)
    rst = open(rstFileName,'w')

    rst.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
    rst.write('<Testrun>\n')
    rst.write('<Environment>\n')
    rst.write('<HostName>'+socket.gethostname()+'</HostName>\n')
    rst.write('<MakeSpec>symbian-'+Enviroment+'</MakeSpec>\n')
    rst.write('</Environment>\n')
    rst.write(time.strftime('<Timestamp date="%Y-%m-%d" time="%H:%M:%S"/>\n',timestamp))


    for root, dirs, files in os.walk(RstPath):
        for name in files:
            if not re.search('result_qt_WITH_', name ) and \
                re.match(name.split('.')[1], 'xml'):
                rst.write('<TestCase name="'+name.split('.')[0]+'">\n')
                path = root+'/'+name
                temp = open(path,'r')
                templines = temp.readlines()
                Validate( templines )
                rst.writelines(templines);
                temp.close()
                rst.write('</TestCase>\n')

    rst.write('</Testrun>\n')
    rst.close

def Validate(lines):
    regexp1 = re.compile('\s*</TestFunction>\s*')
    regexp2 = re.compile('\s*<TestFunction name=\s*')
    regexp1Flag = False
    regexp2Flag = False
    for index, line in enumerate(lines):
        if len(line) > 0 and regexp1.search(line):
            if regexp1Flag:
                lines[index] = ''
            else:
                regexp1Flag = True
        elif len(line) > 0 and regexp2.search(line):
            if regexp2Flag:
                lines[index] = ''
            else:
                regexp2Flag = True
        else:
            regexp1Flag = False
            regexp2Flag = False


if __name__ == '__main__':
    Enviroment = sys.argv[1]
    RstPath = sys.argv[2]
    QtVersion = sys.argv[3]

    JoinResults()

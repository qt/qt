
#include <QtGui>

int main(int argv, char **args)
{
    QApplication app(argv, args);

  {
//![0]
    QStateMachine machine;
    machine.setGlobalRestorePolicy(QStateMachine::RestoreProperties);
//![0]

//![1]
    QState *s1 = new QState();
    s1->assignProperty(object, "fooBar", 1.0);
    machine.addState(s1);
    machine.setInitialState(s1);
      
    QState *s2 = new QState();  
    machine.addState(s2);
//![1]
  }

  {

//![2]
    QStateMachine machine;
    machine.setGlobalRestorePolicy(QStateMachine::RestoreProperties);
      
    QState *s1 = new QState();
    s1->assignProperty(object, "fooBar", 1.0);
    machine.addState(s1);
    machine.setInitialState(s1);
      
    QState *s2 = new QState(s1);  
    s2->assignProperty(object, "fooBar", 2.0);
    s1->setInitialState(s2);
          
    QState *s3 = new QState(s1);
//![2]

  }

  {
//![3]
    QState *s1 = new QState();
    QState *s2 = new QState();      
    
    s1->assignProperty(button, "geometry", QRectF(0, 0, 50, 50));
    s2->assignProperty(button, "geometry", QRectF(0, 0, 100, 100));
    
    s1->addTransition(button, SIGNAL(clicked()), s2);
//![3]

  }

  {
//![4]
    QState *s1 = new QState();
    QState *s2 = new QState();      
    
    s1->assignProperty(button, "geometry", QRectF(0, 0, 50, 50));
    s2->assignProperty(button, "geometry", QRectF(0, 0, 100, 100));
    
    QSignalTransition *transition = s1->addTransition(button, SIGNAL(clicked()), s2);
    transition->addAnimation(new QPropertyAnimation(button, "geometry"));
//![4]

  }

  {

//![5]
    QState *s1 = new QState();
    s1->assignProperty(button, "geometry", QRectF(0, 0, 50, 50));
    
    QState *s2 = new QState();
    
    s1->addTransition(s1, SIGNAL(polished()), s2);
//![5]

  }

  {

//![6]
    QState *s1 = new QState();
    QState *s2 = new QState();
    
    s2->assignProperty(object, "fooBar", 2.0);
    s1->addTransition(s2);    
    
    QStateMachine machine;
    machine.setInitialState(s1);
    machine.addDefaultAnimation(new QPropertyAnimation(object, "fooBar"));    
//![6]

  }

    return app.exec();
}


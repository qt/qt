#ifndef ANIMATIONDIALOG_H
#define ANIMATIONDIALOG_H

#include <QDialog>
#include <QMessageBox>

class QSpinBox;
class QLineEdit;
class StickMan;
class Animation;
class AnimationDialog: public QDialog
{
    Q_OBJECT
public:
    AnimationDialog(StickMan *stickMan, QWidget *parent = 0);
    ~AnimationDialog();

public slots:    
    void currentFrameChanged(int currentFrame);
    void totalFramesChanged(int totalFrames);
    void setCurrentAnimationName(const QString &name);

    void newAnimation();
    void saveAnimation();
    void loadAnimation();

private:
    void saveCurrentFrame();
    void stickManFromCurrentFrame();
    void initFromAnimation();
    void initUi();
    QMessageBox::StandardButton maybeSave();

    QSpinBox *m_currentFrame;
    QSpinBox *m_totalFrames;
    QLineEdit *m_name;
    Animation *m_animation;
    StickMan *m_stickman;
};
 
#endif

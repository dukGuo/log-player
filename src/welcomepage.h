#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
class MainWindow;

class WelcomePage : public QWidget
{
    Q_OBJECT
public:
    explicit WelcomePage(MainWindow *parent = nullptr);

private:
    QPushButton* createLabel(QString text);

signals:

protected:
    void showEvent(QShowEvent*) override;
private:
    MainWindow* mMainWnd;
    QString mTitleStyle;
    QWidget* mWidget{nullptr};
};

#endif // WELCOMEPAGE_H

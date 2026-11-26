#ifndef ADAPTIVEFIELD_H
#define ADAPTIVEFIELD_H

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSizePolicy>

class QResizeEvent; // forward-declaration: в .h обращаемся только к указателю

// QLineEdit, который логирует изменение собственного размера и позволяет
// менять горизонтальную политику на лету.
// Q_OBJECT НЕ нужен: resizeEvent – виртуальный метод QWidget, а не сигнал/слот.
class AdaptiveLineEdit : public QLineEdit
{
public:
    explicit AdaptiveLineEdit(const QString &label, QWidget *parent = nullptr);

    void setHorizontalPolicy(QSizePolicy::Policy hp);
    void setVerticalPolicy(QSizePolicy::Policy vp);

    QString label() const { return m_label; }

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QString m_label; // человекочитаемое имя для логов
};

// QPlainTextEdit с той же идеей: лог resizeEvent + смена политик.
class AdaptiveTextEdit : public QPlainTextEdit
{
public:
    explicit AdaptiveTextEdit(const QString &label, QWidget *parent = nullptr);

    void setHorizontalPolicy(QSizePolicy::Policy hp);
    void setVerticalPolicy(QSizePolicy::Policy vp);

    QString label() const { return m_label; }

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QString m_label;
};

#endif // ADAPTIVEFIELD_H
#ifndef FORMWINDOW_H
#define FORMWINDOW_H

#include <QWidget>

#include "adaptivefield.h"

class QFormLayout;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QComboBox;
class QCheckBox;
class QPushButton;

class FormWindow : public QWidget
{
    Q_OBJECT // нужен: здесь есть слоты кнопок (в отличие от adaptivefield)

public:
    explicit FormWindow(QWidget *parent = nullptr);
    ~FormWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void setCompactMode();            // уменьшить margins/spacing
    void setComfortableMode();        // увеличить margins/spacing
    void toggleDescriptionPolicy();   // описание: Expanding <-> Fixed (верт.)
    void toggleAgePolicy();           // возраст: Fixed <-> Expanding (гориз.)
    void dumpSizePolicies();          // вывести политики всех отслеживаемых полей
    void dumpSizes();                 // вывести size/sizeHint/min всех полей
    void onAccept();                  // кнопка OK
    void onReject();                  // кнопка Отмена

private:
    void setupUi();

    // флаг -> имя строкой, для человекочитаемого дампа
    static QString policyName(QSizePolicy::Policy p);

    // --- Поля формы ---
    AdaptiveLineEdit m_name;
    AdaptiveLineEdit m_email;
    AdaptiveLineEdit m_age;
    QComboBox *m_country;
    AdaptiveTextEdit m_description;
    QCheckBox *m_agree;

    // --- Layout ---
    QVBoxLayout *m_rootLayout;    // основной вертикальный
    QFormLayout *m_formLayout;    // анкета «метка: поле»

    // Список виджетов для дампа политик/размеров
    QList<QWidget *> m_trackedFields;

    // Состояние переключателей политик
    bool m_descriptionExpanding;  // текущая верт. политика описания
    bool m_ageExpanding;          // текущая гориз. политика возраста
};

#endif // FORMWINDOW_H
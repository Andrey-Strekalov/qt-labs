#include "formwindow.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QVBoxLayout>

// ---------- Конструктор / деструктор ----------

FormWindow::FormWindow(QWidget *parent)
    : QWidget(parent)
    , m_name("Имя")
    , m_email("Email")
    , m_age("Возраст")
    , m_country(nullptr)
    , m_description("Описание")
    , m_agree(nullptr)
    , m_rootLayout(nullptr)
    , m_formLayout(nullptr)
    , m_descriptionExpanding(true)
    , m_ageExpanding(false)
{
    setupUi();
    setComfortableMode(); // стартовые отступы/зазоры

    qDebug() << "[INIT] окно создано; растяните его и смотрите [RESIZE] в логе";
}

FormWindow::~FormWindow()
{
    qDebug() << "[DESTROY] FormWindow";
}

// ---------- Построение интерфейса ----------

void FormWindow::setupUi()
{
    setWindowTitle(QStringLiteral("Адаптивная форма - QSizePolicy / addStretch / margins"));
    resize(560, 620);

    // Основной вертикальный layout окна.
    // Его margins/spacing мы будем менять кнопками «Компактный/Развёрнутый».
    m_rootLayout = new QVBoxLayout(this);

    auto *title = new QLabel(QStringLiteral("<b>Анкета</b>"), this);
    m_rootLayout->addWidget(title);

    // Форма «метка: поле». Свои margins обнуляем, чтобы общие отступы задавал
    // только m_rootLayout (иначе получили бы двойные отступы).
    m_formLayout = new QFormLayout();
    m_formLayout->setContentsMargins(0, 0, 0, 0);
    m_formLayout->setSpacing(8);
    m_rootLayout->addLayout(m_formLayout);

    // Имя — тянется вширь (Expanding), ввысь фиксировано.
    m_name.setHorizontalPolicy(QSizePolicy::Expanding);
    m_name.setVerticalPolicy(QSizePolicy::Fixed);
    m_name.setPlaceholderText("Иван Иванов");
    m_formLayout->addRow(QStringLiteral("&Имя:"), &m_name);

    // Email — то же поведение.
    m_email.setHorizontalPolicy(QSizePolicy::Expanding);
    m_email.setVerticalPolicy(QSizePolicy::Fixed);
    m_email.setPlaceholderText("ivan@example.com");
    m_formLayout->addRow(QStringLiteral("&Email:"), &m_email);

    // Возраст — по умолчанию зафиксирован по ширине (Fixed) и НЕ тянется.
    // Чтобы в колонке полей он не растянулся на всю ширину, оборачиваем его
    // в QHBoxLayout с addStretch(): поле занимает sizeHint, пружина — остаток.
    m_age.setHorizontalPolicy(QSizePolicy::Fixed);
    m_age.setVerticalPolicy(QSizePolicy::Fixed);
    m_age.setPlaceholderText("лет");
    m_age.setValidator(new QIntValidator(0, 130, &m_age)); // родитель = m_age

    auto *ageRow = new QHBoxLayout();
    ageRow->setContentsMargins(0, 0, 0, 0);
    ageRow->addWidget(&m_age);
    ageRow->addStretch(); // ← демонстрация addStretch в форме
    m_formLayout->addRow(QStringLiteral("&Возраст:"), ageRow);

    // Страна — комбобокс, тянется вширь.
    m_country = new QComboBox(this);
    m_country->addItems({"Россия", "Беларусь", "Казахстан", "Другая"});
    m_country->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_formLayout->addRow(QStringLiteral("&Страна:"), m_country);

    // Описание — многострочник, тянется и вширь, и ввысь: именно оно забирает
    // свободное вертикальное место при растяжении окна (адаптивность по высоте).
    m_description.setHorizontalPolicy(QSizePolicy::Expanding);
    m_description.setVerticalPolicy(QSizePolicy::Expanding);
    m_description.setPlaceholderText("Расскажите о себе…");
    m_formLayout->addRow(QStringLiteral("&Описание:"), &m_description);

    // Согласие — чекбокс, фиксирован по обеим осям.
    m_agree = new QCheckBox(QStringLiteral("Я согласен с условиями"), this);
    m_agree->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_formLayout->addRow(QString(), m_agree); // пустая метка = сдвиг вправо

    // Регистрируем поля для дампа.
    m_trackedFields = { &m_name, &m_email, &m_age, m_country,
                       &m_description, m_agree };

    // --- Панель режимов: отступы и политики ---
    auto *modeRow = new QHBoxLayout();
    auto *btnCompact   = new QPushButton(QStringLiteral("Компактный"), this);
    auto *btnComfy     = new QPushButton(QStringLiteral("Развёрнутый"), this);
    auto *btnDescPol   = new QPushButton(QStringLiteral("Политика описания"), this);
    auto *btnAgePol    = new QPushButton(QStringLiteral("Политика возраста"), this);
    auto *btnDumpPol   = new QPushButton(QStringLiteral("Дамп политик"), this);
    auto *btnDumpSize  = new QPushButton(QStringLiteral("Дамп размеров"), this);
    modeRow->addWidget(btnCompact);
    modeRow->addWidget(btnComfy);
    modeRow->addWidget(btnDescPol);
    modeRow->addWidget(btnAgePol);
    modeRow->addWidget(btnDumpPol);
    modeRow->addWidget(btnDumpSize);
    m_rootLayout->addLayout(modeRow);

    connect(btnCompact,  &QPushButton::clicked, this, &FormWindow::setCompactMode);
    connect(btnComfy,    &QPushButton::clicked, this, &FormWindow::setComfortableMode);
    connect(btnDescPol,  &QPushButton::clicked, this, &FormWindow::toggleDescriptionPolicy);
    connect(btnAgePol,   &QPushButton::clicked, this, &FormWindow::toggleAgePolicy);
    connect(btnDumpPol,  &QPushButton::clicked, this, &FormWindow::dumpSizePolicies);
    connect(btnDumpSize, &QPushButton::clicked, this, &FormWindow::dumpSizes);

    // --- Кнопки действий: прижаты вправо через addStretch ---
    auto *btnRow = new QHBoxLayout();
    auto *btnOk     = new QPushButton(QStringLiteral("OK"), this);
    auto *btnCancel = new QPushButton(QStringLiteral("Отмена"), this);
    btnRow->addStretch(); // ← прижимает кнопки вправо
    btnRow->addWidget(btnOk);
    btnRow->addWidget(btnCancel);
    m_rootLayout->addLayout(btnRow);

    connect(btnOk,     &QPushButton::clicked, this, &FormWindow::onAccept);
    connect(btnCancel, &QPushButton::clicked, this, &FormWindow::onReject);
}

// ---------- Слоты режимов ----------

void FormWindow::setCompactMode()
{
    m_rootLayout->setContentsMargins(4, 4, 4, 4);
    m_rootLayout->setSpacing(4);
    qDebug() << "[MODE] compact: margins=4, spacing=4";
}

void FormWindow::setComfortableMode()
{
    m_rootLayout->setContentsMargins(12, 12, 12, 12);
    m_rootLayout->setSpacing(10);
    qDebug() << "[MODE] comfortable: margins=12, spacing=10";
}

// ---------- Слоты переключения политик ----------

void FormWindow::toggleDescriptionPolicy()
{
    m_descriptionExpanding = !m_descriptionExpanding;
    m_description.setVerticalPolicy(m_descriptionExpanding
                                        ? QSizePolicy::Expanding
                                        : QSizePolicy::Fixed);
    qDebug() << "[TOGGLE] описание vert ->"
             << (m_descriptionExpanding ? "Expanding" : "Fixed");
}

void FormWindow::toggleAgePolicy()
{
    m_ageExpanding = !m_ageExpanding;
    m_age.setHorizontalPolicy(m_ageExpanding
                                  ? QSizePolicy::Expanding
                                  : QSizePolicy::Fixed);
    qDebug() << "[TOGGLE] возраст horiz ->"
             << (m_ageExpanding ? "Expanding" : "Fixed");
}

// ---------- Дампы ----------

void FormWindow::dumpSizePolicies()
{
    qDebug() << "===== [DUMP] size policies =====";
    for (QWidget *w : m_trackedFields) {
        if (!w) continue;
        const QSizePolicy sp = w->sizePolicy();
        qDebug() << "[POLICY]" << w->metaObject()->className()
                 << "H:" << policyName(sp.horizontalPolicy())
                 << "V:" << policyName(sp.verticalPolicy());
    }
}

void FormWindow::dumpSizes()
{
    qDebug() << "===== [DUMP] sizes =====";
    for (QWidget *w : m_trackedFields) {
        if (!w) continue;
        const QSize hint = w->sizeHint();
        const QSize minHint = w->minimumSizeHint();
        qDebug() << "[SIZE]" << w->metaObject()->className()
                 << "size:" << w->width() << "x" << w->height()
                 << "hint:" << hint.width() << "x" << hint.height()
                 << "minHint:" << minHint.width() << "x" << minHint.height();
    }
}

// ---------- Кнопки действий ----------

void FormWindow::onAccept()
{
    QMessageBox::information(this,
                             QStringLiteral("Анкета"),
                             QStringLiteral("Анкета принята.\nИмя: %1\nEmail: %2")
                                 .arg(m_name.text(), m_email.text()));
}

void FormWindow::onReject()
{
    close();
}

// ---------- resizeEvent окна ----------

void FormWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    qDebug() << "[RESIZE] FormWindow ->"
             << event->size().width() << "x" << event->size().height();
}

// ---------- Утилита: имя политики ----------

QString FormWindow::policyName(QSizePolicy::Policy p)
{
    switch (p) {
    case QSizePolicy::Fixed:            return QStringLiteral("Fixed");
    case QSizePolicy::Minimum:          return QStringLiteral("Minimum");
    case QSizePolicy::Maximum:          return QStringLiteral("Maximum");
    case QSizePolicy::Preferred:        return QStringLiteral("Preferred");
    case QSizePolicy::Expanding:        return QStringLiteral("Expanding");
    case QSizePolicy::MinimumExpanding: return QStringLiteral("MinimumExpanding");
    case QSizePolicy::Ignored:          return QStringLiteral("Ignored");
    default:                            return QStringLiteral("Unknown");
    }
}
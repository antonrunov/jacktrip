#include "JTMainWindow.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <iostream>
#include "Settings.h"

using std::cout; using std::endl;

// ----------------------------------------------------------------------------

static void setGridRowVisibility(QGridLayout* layout, int row, bool val)
{
  for (int j=0; j < layout->columnCount(); ++j) {
    QWidget* w = layout->itemAtPosition(row,j) ? layout->itemAtPosition(row,j)->widget() : NULL;
    if (NULL != w) {
      w->setVisible(val);
    }
  }
}

// ----------------------------------------------------------------------------

static QVariant checkConfValue(QSettings& conf, const char* opt, QVariant default_val)
{
  if (conf.contains(opt)) {
    return conf.value(opt);
  }
  conf.setValue(opt, default_val);
  return default_val;
}

// ----------------------------------------------------------------------------

JTMainWindow::JTMainWindow()
{
  QCoreApplication::setOrganizationName("JackTrip");
  QCoreApplication::setApplicationName("jacktripui");
  QSettings conf;

  QVBoxLayout* top_layout = new QVBoxLayout(this);
  // Config
  QLabel* lbl_config = new QLabel("--- Config ---");
  lbl_config->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  top_layout->addWidget(lbl_config);
  int row = 0;
  QGridLayout* conf_layout = new QGridLayout();
  // Sample Rate
  QComboBox* cmb_srate = new QComboBox(this);
  cmb_srate->setEditable(true);
  cmb_srate->setInsertPolicy(QComboBox::NoInsert);
  cmb_srate->addItem("22050");
  cmb_srate->addItem("24000");
  cmb_srate->addItem("44100");
  cmb_srate->addItem("48000");
  cmb_srate->addItem("96000");
  cmb_srate->setCurrentText(checkConfValue(conf, SAMPLE_RATE, 48000).toString());
  connect(cmb_srate->lineEdit(), &QLineEdit::editingFinished,
                      this, [=] () {setIntOption(SAMPLE_RATE, cmb_srate->currentText().toUInt());});
  conf_layout->addWidget(new QLabel("Sample Rate"), row, 0);
  conf_layout->addWidget(cmb_srate, row, 1);
  ++row;
  // Frames per Period
  QComboBox* cmb_frames = new QComboBox(this);
  cmb_frames->addItem("16");
  cmb_frames->addItem("32");
  cmb_frames->addItem("64");
  cmb_frames->addItem("128");
  cmb_frames->addItem("256");
  cmb_frames->addItem("512");
  cmb_frames->addItem("1024");
  cmb_frames->setCurrentText(checkConfValue(conf, FRAMES, 128).toString());
  connect(cmb_frames, QOverload<int>::of(&QComboBox::activated),
                      this, [=] () {setIntOption(FRAMES, cmb_frames->currentText().toUInt());});
  conf_layout->addWidget(new QLabel("Frames/Period"), row, 0);
  conf_layout->addWidget(cmb_frames, row, 1);
  ++row;
  // Queue Length
  QSpinBox* spn_qlength = new QSpinBox(this);
  spn_qlength->setRange(2, 100);
  spn_qlength->setValue(checkConfValue(conf, QUEUE_LEN, 2).toUInt());
  connect(spn_qlength, QOverload<int>::of(&QSpinBox::valueChanged),
                      this, [=] () {setIntOption(QUEUE_LEN, spn_qlength->value());});
  conf_layout->addWidget(new QLabel("Queue Length"), row, 0);
  conf_layout->addWidget(spn_qlength, row, 1);
  ++row;
  // Number of Channels
  QSpinBox* spn_channels = new QSpinBox(this);
  spn_channels->setRange(1, 16);
  spn_channels->setValue(checkConfValue(conf, CHANNELS, 2).toUInt());
  connect(spn_channels, QOverload<int>::of(&QSpinBox::valueChanged),
                      this, [=] () {setIntOption(CHANNELS, spn_channels->value()); });
  conf_layout->addWidget(new QLabel("Channels"), row, 0);
  conf_layout->addWidget(spn_channels, row, 1);
  ++row;
  // Mode
  QComboBox* cmb_mode = new QComboBox(this);
  cmb_mode->addItem("Server");
  cmb_mode->addItem("Client");
  cmb_mode->addItem("Hub Client");
  cmb_mode->addItem("Hub Server");
  cmb_mode->setCurrentIndex(checkConfValue(conf, JACKTRIP_MODE, 1).toUInt());
  connect(cmb_mode, QOverload<int>::of(&QComboBox::activated), this, [=] ()
      {
        setIntOption(JACKTRIP_MODE, cmb_mode->currentIndex());
        bool client = cmb_mode->currentIndex() == 1 || cmb_mode->currentIndex() == 2;
        setGridRowVisibility(conf_layout, row+1, client);
      });
  conf_layout->addWidget(new QLabel("Connection Mode"), row, 0);
  conf_layout->addWidget(cmb_mode, row, 1);
  ++row;
  // Remote Address
  QLineEdit* rem_address = new QLineEdit();
  rem_address->setText(checkConfValue(conf, REMOTE_ADDRESS, "").toString());
  connect(rem_address, &QLineEdit::editingFinished,
                      this, [=] () {setStringOption(REMOTE_ADDRESS, rem_address->text());});
  conf_layout->addWidget(new QLabel("Remote Address"), row, 0);
  conf_layout->addWidget(rem_address, row, 1);
  {
    bool client = cmb_mode->currentIndex() == 1 || cmb_mode->currentIndex() == 2;
    setGridRowVisibility(conf_layout, row, client);
  }
  ++row;
  // Advanced Options
  QCheckBox* chk_advanced = new QCheckBox("Advanced settings for jack and jacktrip", this);
  chk_advanced->setChecked(checkConfValue(conf, SHOW_EXTRA_OPTS, 0).toBool());
  connect(chk_advanced, &QCheckBox::toggled, this, [=] () {
      setIntOption(SHOW_EXTRA_OPTS, chk_advanced->isChecked() ? 1 : 0);
      setGridRowVisibility(conf_layout, row+1, chk_advanced->isChecked());
      setGridRowVisibility(conf_layout, row+2, chk_advanced->isChecked());
      setGridRowVisibility(conf_layout, row+3, chk_advanced->isChecked());
      });
  conf_layout->addWidget(chk_advanced, row, 0, 1, 2);
  ++row;
  // Extra Options for Jacktrip
  QLineEdit* extopt_jacktrip = new QLineEdit();
  extopt_jacktrip->setText(checkConfValue(conf, EXTRA_OPTS, "").toString());
  connect(extopt_jacktrip, &QLineEdit::editingFinished,
                      this, [=] () {setStringOption(EXTRA_OPTS, extopt_jacktrip->text());});
  conf_layout->addWidget(new QLabel("Jacktrip Options"), row, 0);
  conf_layout->addWidget(extopt_jacktrip, row, 1);
  setGridRowVisibility(conf_layout, row, chk_advanced->isChecked());
  ++row;
  // Extra Options for Jackd
  QLineEdit* extopt_jackd = new QLineEdit();
  extopt_jackd->setText(checkConfValue(conf, EXTRA_OPTS_JACKD, "").toString());
  connect(extopt_jackd, &QLineEdit::editingFinished,
                      this, [=] () {setStringOption(EXTRA_OPTS_JACKD, extopt_jackd->text());});
  conf_layout->addWidget(new QLabel("Jackd Options"), row, 0);
  conf_layout->addWidget(extopt_jackd, row, 1);
  setGridRowVisibility(conf_layout, row, chk_advanced->isChecked());
  ++row;
  // Extra Options for Backend
  QLineEdit* extopt_backend = new QLineEdit();
  extopt_backend->setText(checkConfValue(conf, EXTRA_OPTS_BACKEND, "").toString());
  connect(extopt_backend, &QLineEdit::editingFinished,
                      this, [=] () {setStringOption(EXTRA_OPTS_BACKEND, extopt_backend->text());});
  conf_layout->addWidget(new QLabel("Backend Options"), row, 0);
  conf_layout->addWidget(extopt_backend, row, 1);
  setGridRowVisibility(conf_layout, row, chk_advanced->isChecked());
  ++row;

  conf_layout->setColumnStretch(1, 1);
  top_layout->addLayout(conf_layout);

  // Action buttons
  top_layout->addStretch();
  top_layout->addSpacing(10);
  QLabel* lbl_command = new QLabel("--- Command ---");
  lbl_command->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  top_layout->addWidget(lbl_command);
  QHBoxLayout* btn_layout = new QHBoxLayout();

  m_btnStart = new QPushButton("Start");
  connect(m_btnStart, &QPushButton::clicked, this, &JTMainWindow::start);
  btn_layout->addWidget(m_btnStart);

  m_btnStop = new QPushButton("Stop");
  connect(m_btnStop, &QPushButton::clicked, this, &JTMainWindow::stop);
  m_btnStop->setEnabled(false);
  btn_layout->addWidget(m_btnStop);

  top_layout->addLayout(btn_layout);

  QLabel* lbl_status = new QLabel("--- Status ---");
  lbl_status->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  top_layout->addWidget(lbl_status);

  m_status = new QLabel("Stopped");
  QFontMetrics fm(m_status->font() );
  m_status->setMinimumHeight(fm.lineSpacing()*3);
  top_layout->addWidget(m_status);

  QSize sz = conf.value(WINDOW_SIZE, QSize(0,0)).toSize();
  if (!sz.isNull()) {
    resize(sz);
  }

  m_settings = NULL;
}

// ----------------------------------------------------------------------------

void JTMainWindow::start()
{
  if (NULL != m_settings) {
    cout << "already started" << endl;
    return;
  }
  cout << "start" << endl;
  QFile jackdrc(QDir::homePath() + "/.jackdrc");
  if (!jackdrc.open(QIODevice::WriteOnly)) {
    cout << "failed to open .jackdrc (" << jackdrc.fileName().toStdString() << ")" << endl;
    return;
  }
  QSettings conf;
#if defined(Q_OS_LINUX)
  jackdrc.write("/usr/bin/jackd -R");
#elif defined(Q_OS_MACOS)
  jackdrc.write("/usr/local/bin/jackd -R");
#endif
  if (conf.value(SHOW_EXTRA_OPTS).toBool() && !conf.value(EXTRA_OPTS_JACKD).toString().isEmpty()) {
    jackdrc.write(" "); jackdrc.write(conf.value(EXTRA_OPTS_JACKD).toString().toLocal8Bit());
  }
#if defined(Q_OS_LINUX)
  jackdrc.write(" -dalsa");
  jackdrc.write(" -r"); jackdrc.write(conf.value(SAMPLE_RATE).toString().toLocal8Bit());
  jackdrc.write(" -p"); jackdrc.write(conf.value(FRAMES).toString().toLocal8Bit());
#elif defined(Q_OS_MACOS)
  jackdrc.write(" -dcoreaudio");
  jackdrc.write(" -r"); jackdrc.write(conf.value(SAMPLE_RATE).toString().toLocal8Bit());
  jackdrc.write(" -p"); jackdrc.write(conf.value(FRAMES).toString().toLocal8Bit());
#endif
  if (conf.value(SHOW_EXTRA_OPTS).toBool() && !conf.value(EXTRA_OPTS_BACKEND).toString().isEmpty()) {
    jackdrc.write(" "); jackdrc.write(conf.value(EXTRA_OPTS_BACKEND).toString().toLocal8Bit());
  }
  jackdrc.write("\n");
  jackdrc.close();
  QStringList jacktrip_opts;
  switch (conf.value(JACKTRIP_MODE).toInt()) {
    case 0: jacktrip_opts << "-s"; break;
    case 1: jacktrip_opts << "-c" << conf.value(REMOTE_ADDRESS).toString(); break;
    case 2: jacktrip_opts << "-C" << conf.value(REMOTE_ADDRESS).toString(); break;
    case 3: jacktrip_opts << "-S"; break;
    default:
            cout << "invalid mode " << conf.value(JACKTRIP_MODE).toInt() << endl;
            return;
  }
  jacktrip_opts << "-q" << conf.value(QUEUE_LEN).toString();
  jacktrip_opts << "-n" << conf.value(CHANNELS).toString();
  if (conf.value(SHOW_EXTRA_OPTS).toBool()) {
    jacktrip_opts << conf.value(EXTRA_OPTS).toString().split(' ', QString::SkipEmptyParts);
  }
  cout << jacktrip_opts.size() << " params" << endl;

  m_settings = new Settings;
  try
  {
    int n = 1 + jacktrip_opts.size();
    char* argv[n];
    argv[0] = NULL;
    std::vector<std::string> args(n);
    for (int j=1 ; j<n; ++j) {
      args[j] = jacktrip_opts[j-1].toStdString();
      argv[j] = (char*)args[j].c_str();
      cout << "opt: " << args[j] << endl;
    }
    m_settings->parseInput(n, argv);
    //settings->startJackTrip();
  }
  catch (const std::exception & e)
  {
    std::cerr << "ERROR:" << std::endl;
    std::cerr << e.what() << std::endl;
    std::cerr << "Exiting JackTrip..." << std::endl;
    std::cerr << gPrintSeparator << std::endl;
    delete m_settings;
    m_settings = NULL;
    return;
  }
  m_btnStart->setEnabled(false);
  m_btnStop->setEnabled(true);
  m_status->setText("Starting");
  connect(m_settings, &QThread::finished, this, &JTMainWindow::stop);
  connect(m_settings, &Settings::statusChanged, this, &JTMainWindow::updateStatus);
  m_settings->start();
}

// ----------------------------------------------------------------------------

void JTMainWindow::stop()
{
  if (NULL == m_settings) {
    cout << "not started" << endl;
    return;
  }
  cout << "stop" << endl;
  m_settings->stopJackTrip();
  m_settings->wait();
  delete m_settings;
  m_settings = NULL;

  m_btnStart->setEnabled(true);
  m_btnStop->setEnabled(false);
  m_status->setText("Stopped");
}

// ----------------------------------------------------------------------------

void JTMainWindow::updateStatus(int status, const QString& msg)
{
  m_status->setText(msg);
}

// ----------------------------------------------------------------------------

void JTMainWindow::setIntOption(const char* opt, int val)
{
  QSettings conf;
  if (conf.value(opt) != val) {
    cout << opt << " " << val << endl;
    conf.setValue(opt, val);
  }
}

// ----------------------------------------------------------------------------

void JTMainWindow::setStringOption(const char* opt, const QString& val)
{
  QSettings conf;
  if (conf.value(opt) != val) {
    cout << opt << " " << val.toStdString() << endl;
    conf.setValue(opt, val);
  }
}

// ----------------------------------------------------------------------------

void JTMainWindow::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  QSettings conf;
  conf.setValue(WINDOW_SIZE, size());
}

// ----------------------------------------------------------------------------


#include <QWidget>

class Settings;
class QPushButton;
class QLabel;

class JTMainWindow : public QWidget
{
  public:
    JTMainWindow();

  protected:
    void start();
    void stop();
    void updateStatus(int status, const QString& msg);

  protected:
    Settings* m_settings;
    QPushButton* m_btnStart;
    QPushButton* m_btnStop;
    QLabel*      m_status;

  protected:
    void resizeEvent(QResizeEvent *event);
    void setIntOption(const char* opt, int val);
    void setStringOption(const char* opt, const QString& val);

  protected:
    static constexpr const char* SAMPLE_RATE = "SampleRate";
    static constexpr const char* FRAMES = "Frames";
    static constexpr const char* QUEUE_LEN = "QueueLength";
    static constexpr const char* CHANNELS = "NumChannels";
    static constexpr const char* JACKTRIP_MODE = "JackTripMode";
    static constexpr const char* REMOTE_ADDRESS = "RemoteAddress";
    static constexpr const char* SHOW_EXTRA_OPTS = "ShowExtraOpts";
    static constexpr const char* EXTRA_OPTS = "ExtraOpts";
    static constexpr const char* EXTRA_OPTS_JACKD = "OptsJackd";
    static constexpr const char* EXTRA_OPTS_BACKEND = "OptsBackend";
    static constexpr const char* WINDOW_SIZE = "WindowSize";
};

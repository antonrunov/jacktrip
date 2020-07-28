#include <QWidget>

class Settings;

class JTMainWindow : public QWidget
{
  public:
    JTMainWindow();

  protected:
    void start();
    void stop();

  protected:
    Settings* m_settings;

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

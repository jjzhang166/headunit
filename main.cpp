#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QCommandLineParser>
#include <QDebug>
#include <headunit/hu_gst.h>

/**
 * @brief Setup, parse command line arguments then set the settings for
 * headunit.
 *        If the help flag is passed then it returns 1 otherwise it returns 0
 * @param a
 * @return int
 */
int cmd_parser(QApplication *a) {
  QStringList arguments = QCoreApplication::arguments();

  QCommandLineParser parser;
  parser.setApplicationDescription("Qt5 based Android Auto (tm) client.");
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption certificateOption(
      QStringList() << "c"
                    << "certificate",
      QCoreApplication::translate("main",
                                  "Load the the SSL certificate from <file>"),
      QCoreApplication::translate("main", "file"));
  parser.addOption(certificateOption);

  QCommandLineOption privateKeyOption(
      QStringList() << "k"
                    << "private-key",
      QCoreApplication::translate("main",
                                  "Load the the SSL private key from <file>\n"),
      QCoreApplication::translate("main", "file"));
  parser.addOption(privateKeyOption);

  QCommandLineOption alsaInputOption(
      QStringList() << "alsa-input",
      QCoreApplication::translate(
          "main", "Set the ALSA input <device>, otherwise using default"),
      QCoreApplication::translate("main", "device"));
  parser.addOption(alsaInputOption);

  QCommandLineOption alsaOutputOption(
      QStringList() << "alsa-output",
      QCoreApplication::translate(
          "main", "Set the ALSA output <device>, otherwise using default\n"),
      QCoreApplication::translate("main", "device"));
  parser.addOption(alsaOutputOption);

  QCommandLineOption microphoneOption(
      QStringList() << "enable-mic",
      QCoreApplication::translate(
          "main", "Enable/disable microphone, enabled by default"));
  parser.addOption(microphoneOption);
  parser.process(*a);

  hu_set_settings(parser.value(privateKeyOption).toLatin1().data(),
                  parser.value(certificateOption).toLatin1().data());

  hu_gst_set_settings(true, parser.value(alsaInputOption).toLatin1().data(),
                      parser.value(alsaOutputOption).toLatin1().data());

  if (arguments.contains("--help") || arguments.contains("-h") ||
      arguments.contains("-?")) {
    return 1;
  } else {
    return 0;
  }
}

/**
 * @brief
 *
 * @param argc
 * @param argv[]
 * @return int
 */
int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QCoreApplication::setOrganizationName("ViktorGino");
  QCoreApplication::setOrganizationDomain("viktorgino.github.io");
  QCoreApplication::setApplicationName("HeadUnit Desktop");
  QCoreApplication::setApplicationVersion("1.0");

  /*qDebug("\n\n-------------------------------------------\n");
  qDebug() << "alsa_input = " << hu_gst_settings.alsa_input
           << " | alsa_output = " << hu_gst_settings.alsa_output
           << " | certificate_file = "
           << hu_settings.certificate_file
           << " | privateKey_file = "
           << hu_settings.privateKey_file << "\n";
  qDebug("-------------------------------------------\n\n");*/

  // qDebug() <<"\n The value of mic option is : "<<
  // parser.value(microphoneOption);
  if (cmd_parser(&a)) {
    return 0;
  }
  MainWindow w;
  w.show();
  return a.exec();
}

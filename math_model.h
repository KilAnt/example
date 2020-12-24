#ifndef MATH_MODEL_H
#define MATH_MODEL_H

#include <QMainWindow>
#include "abstract_device.h"
#include "abstarct_json.h"
#include "multikreader.h"

# define M_PI           3.14159265358979323846  /* pi */

struct JoystickData
{
    double   elevator;   //!< руль высоты        (-1..+1)
    double   aileron;    //!< эйлероны           (-1..+1)
    double   throttle;   //!< газ                ( 0..+1) +1 - максимум
    double   breaks;     //!< тормоз             ( 0..+1) +1 - зажат
    double   rudder;     //!< руль направления   (-1..+1)
    double   flaps ;      //!< закрылки,      0, 1, 2
    double   buttons=0;  //!< 0й бит: старт мат модели
    JoystickData() : elevator(0), aileron(0), throttle(0), breaks(0), rudder(0), flaps(1) {}
};

struct modelData{
    double speedX;          //!< Продольная составляющая земной скорости в ЛКС
    double speedY;          //!< Вертикальная составляющая земной скорости в ЛКС
    double speedZ;          //!< Боковая составляющая земной скорости в ЛКС

    double rollAngle;       //!< Крен, радиан
    double pitchAngle;      //!< Тангаж, радиан
    double trueCourse;      //!< Курс, радиан
    double magnCourse;      //!< Курс магнитный, радиан

    double latitude;        //!< Широта в WGS-84, радианы
    double longitude;       //!< Долгота в WGS-84, радианы
    double altitude;        //!< Геодезическая высота WGS84, метры

    double Ny;              //!< Вертикальная перегрузка в ССК, g
    double Nz;              //!< Боковая перегрузка в ССК, g
    double Nx;              //!< Продольная перегрузка в ССК, g

    double absAltitude;                 //!< Абсолютная высота, м
    double relAltitude1;                //!< Относительная высота 1, м

    double climbRate;                   //!< Вертикальная скорость, м/с
    double indicatedSpeed;              //!< Приборная скорость, м/с

    double machNumber;                  //!< Число Маха
    double trueSpeed;                   //!< Истинная скорость, м/с

    double outdoorTemperature;          //!< Температура наружного воздуха, градусы Цельсия
    double brakingTemperature;          //!< Температура торможения, градусы Цельсия
    double dynamicPressure;             //!< Динамичаское давление, мБар
    double fullPressure;                //!< Полное давление, мБар
    double baroCorrection1;             //!< Барокоррекция 1, мБар

    double attackAngleLeft;             //!< Угол атаки левый, градусы
    double attackAngleRight;            //!< Угол атаки правый, градусы

    double absAltitudeBar; //!< Абсолютная барометрическая (геопотенциальная) высота, м
    double relAltitude; //!< Относительная барометрическая высота, м
    double Vyb;         //!< Вертикальная барометрическая скорость, м/с
    double Vpr;         //!< Приборная скорость, км/ч
    double PsiGM;		//!< Курс гиромагнитный, град

    double nz;          //!< Ускорение боковое (скольжение), м/с2
    double W_X;         //!< Угловая скорость по оси X, град/с
    double W_Y;         //!< Угловая скорость по оси Y, град/с
    double W_Z;         //!< Угловая скорость по оси Z, град/с

    double roll_korr;	//!< Крен корректируемый, град
    double pitch_korr;	//!< Тангаж корректируемый, град


    double turnKVD; //обороты КВД
    double turnKND;//обороты КНД
    double temperGases;
    double temperOil;
    double pressOil;
    double pressFuel;
    double vibration;
    double pointerPosition;
    double deviationCPGO;
    double deviationRN;
    double deviationElerons;
    double deviationFlaps;
    double positionRUD;
    double driftAngle;
    double fuelLevel;
    double altituteRadio;

};

struct DynState
{
    double lat;     //!< широта  WGS-84, градус
    double lon;     //!< долгота WGS-84, градус
    float alt;      //!< высота  WGS-84, метры (MSL)
    float Yagl;     //!< истинная высота, м (AGL)

    float rollRate; //!< скорость изменения крена, градус/с
    float roll;     //!< крен, градусы
    float pitch;    //!< тангаж, градусы
    float head;     //!< курс, градусы
    float track;    //!< Истинный путевой угол, градус

    float gs;       //!< земная скорость, м/с
    float Vu;       //!< вертикальная скорость, м/с
    float tas;      //!< true airspeed, м/с
    float cas;      //!< calibrated airspeed, м/с

    float windSpeed;     //!< скорость ветра, м/с
    float windDirection; //!< направление навигационного ветра, градус

    bool  isOnWheels; //!< признак обжатия шасси

    int   rpm;        //!< обороты двигателя

    float   attack;     //!< угол атаки, градус

    float   Vx, Vy, Vz;

    bool isValid;

    // DynState() : isValid(false) {}
    DynState() { memset(this,0, sizeof(DynState)); }
};


QT_BEGIN_NAMESPACE
namespace Ui { class math_model; }
QT_END_NAMESPACE

class math_model : public QMainWindow
{
    Q_OBJECT

public:
    math_model(QWidget *parent = nullptr);
    ~math_model();

private:
    Ui::math_model *ui;
    ///Порт, на который приходят данные из OBS
    static const quint16 port_r = 30002;
    ///Порт, на который отправляются данные в OBS
    static const quint16 port_s = 20002;
    ///Порт, на который приходят данные из модели матлаба
    static const quint16 port_r_model = 30003;
    ///Порт, на который приходят данные из модели С172
    static const quint16 port_r_172 = 40033;
    ///Порт, на который приходят данные в модель С172
    static const quint16 port_s_172 = 40034;
    ///Порт, на который приходят данные в модели матлаба
    static const quint16 port_send_to_model = 55555;
    Abstarct_json *JSON_for_read;
    /// Файл с телеметрией полета
    const QString multik = "multik.json";
    ///
    /// \brief ad Класс абстрактного устройства
    ///
    abstract_device *ad;
    ///
    /// \brief make_query_list Формирование списка для запроса данных из OBS
    /// \return
    ///
    QJsonArray make_query_list();
    /// Принимает данные от модели С172
    class reciver *recive_172;
    /// Принимает данные от модели матлаб
    class reciver *recive_model;
    ///Буфер, куда загружается файл с JSON-телеметрией полета
    QJsonDocument memory_for_multik;
    /// Счетчик, последовательно перебирающий массив объектов в файле с JSON-телеметрией полета
    int json_array_counter = NULL;
    /// Состояние модели (ВКЛ/ВЫКЛ)
    bool check_enable_model = false;
    ///
    /// \brief send_to_model Отправляет в матмодель данные, полученные из OBS
    /// \param object JSON-объект данных из OBS
    ///
    void send_to_model(QJsonObject &object);
    /// Таймер, по которому отправляется кадр данных из файла, имитирующий работу модели
    QTimer time_to_multiki;
    /// Объект класса данных из С172
    modelData *mData;
    /// Объект класса для чтения телеметрии полёта
    MultikReader *mr;
    /// Инициализация устройства
    void initialize();
    QJsonParseError ErrorJSON;
private slots:
    ///
    /// \brief data_from_172 Отправляет в OBS данные, полученные из модели С172
    /// \param datagram датаграма из сокета, полученная из модели
    ///
    void data_from_172(QByteArray &datagram);
    ///
    /// \brief send_data_from_file Отправляет в OBS данные, считанные из файла
    ///
    void send_data_from_file();
    ///
    /// \brief fromModelToJSON Отправляет данные, полученные из сокета, в OBS
    /// \param datagram данные от модели матлаба
    ///
    void fromModelToJSON(QByteArray &datagram);
    ///
    /// \brief mult Отправляет в OBS данные, считанные из файла
    /// \param json считанный кадр в виде JSON-сообщения
    ///
    void mult(QByteArray json);

    /// \brief onCurrentFrame Получает значение времени текущего кадра и перемещает сладер на форие
    void onCurrentFrame(int frame);
    void onReaded();
    ///
    /// \brief control - Обработчик заданий из OBS
    /// \param message - тип задания
    ///
    void control(message_to_ui message);

    void on_startReadButton_clicked();
    void on_stopReadButton_clicked();
    void on_timeJumpButton_clicked();
    void on_timeSlider_sliderReleased();
    void on_selectFileButton_clicked();
    void on_timeSpin_editingFinished();
    void on_frameRB_toggled(bool checked);
    void on_frameJumpButton_clicked();
    void on_pushButton_clicked();
    void startBroadcast();
    void connect_ui(QByteArray json);
    void on_checkBox_toggled(bool checked);
};
#endif // MATH_MODEL_H

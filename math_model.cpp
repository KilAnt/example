#include "math_model.h"
#include "ui_math_model.h"

math_model::math_model(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::math_model)
{
    ui->setupUi(this);
    ui->textBrowser->setDisabled(true);
    initialize();
    ad->startBroadcasting();
    connect(ad, &abstract_device::ui_signal, this, &math_model::control);
    connect(mr, SIGNAL(sendData(QByteArray)), this, SLOT(mult(QByteArray)));
    connect(mr, SIGNAL(currentFrame(int)), this, SLOT(onCurrentFrame(int)));
    connect(mr, SIGNAL(readed()), this, SLOT(onReaded()));
    connect(&time_to_multiki, &QTimer::timeout, this, &math_model::send_data_from_file);
    connect(recive_model, SIGNAL(getData(QByteArray &)), this, SLOT(fromModelToJSON(QByteArray &)));
    connect(recive_172, SIGNAL(getData(QByteArray &)), this, SLOT(data_from_172(QByteArray &)));
}

math_model::~math_model()
{
    delete ui;
    delete mr;
}

void math_model::control(message_to_ui message)
{
    switch (message)
    {
    case INIT: ui->state->setText("Inited");
        break;
    case QUERY: ui->state->setText("Quered");
        break;
    case RESTART: ui->state->setText("Restarted");
        on_pushButton_clicked();
        break;
    case CLOSE: ui->state->setText("Closed");
        ad->CloseApp();
        break;
    }
}

void math_model::connect_ui(QByteArray json)
{
    ui->textBrowser->setText(QJsonDocument::fromJson(json, &ErrorJSON).toJson(QJsonDocument::Indented));
}
void math_model::initialize()
{
    ad = new abstract_device(make_query_list(), "math_model", 30002, 20002, this);
    recive_model = new reciver(port_r_model, this);
    recive_172 = new reciver(port_r_172, this);
    mr = new MultikReader();

}

QJsonArray math_model::make_query_list()
{
    QJsonArray textsArray;
    textsArray.append("JOYS_Throttle_buttons");
    textsArray.append("JOYS_Throttle_U");
    textsArray.append("JOYS_Throttle_X");
    textsArray.append("JOYS_Throttle_Y");
    textsArray.append("JOYS_Throttle_R");
    textsArray.append("JOYS_Joystick_Y");
    textsArray.append("JOYS_Joystick_X");
    return textsArray;
}

void math_model::mult(QByteArray json)
{
    //    ui->outText->setText(QJsonDocument::fromJson(json).toJson(QJsonDocument::Indented));
    ad->send->broadcastDatagram(json, QHostAddress::LocalHost, port_s);
}

void math_model::onCurrentFrame(int frame)
{
    if(!ui->timeSlider->isSliderDown())
        ui->timeSlider->setValue(frame);
    if(ui->frameRB->isChecked())
        ui->currentTimeLabel->setText(QString::number(mr->getTickVector()->at(frame)));
    if(ui->timeRB->isChecked())
        ui->currentTimeLabel->setText(QString::number(mr->getTimeVector()->at(frame).hour())+":"+QString::number(mr->getTimeVector()->at(frame).minute())+":"+QString::number(mr->getTimeVector()->at(frame).second()));
}

void math_model::send_data_from_file()
{
    //ui->textBrowser->append(doc.object())
    //qDebug()<< memory_for_multik.object().find("oldData").value().toArray().at(json_array_counter).toObject();            //.array().takeAt(json_array_counter).toObject();
    int max_obj = memory_for_multik.object().find("oldData").value().toArray().count();
    QByteArray sendArr = QJsonDocument(memory_for_multik.object().find("oldData").value().toArray().at(json_array_counter).toObject()).toJson(QJsonDocument::Indented);
    ++json_array_counter;
    if(max_obj == json_array_counter)
    {
        json_array_counter = 0;
    }
    ad->send->broadcastDatagram(sendArr, QHostAddress::LocalHost, port_s);
}

void math_model::data_from_172(QByteArray &datagram)
{
    check_enable_model = true;
    QJsonObject fModel;
    DynState *acs;
    acs = reinterpret_cast<DynState*>(datagram.data());
    //qDebug() <<(acs->rpm+30);
    fModel.insert("altitude", acs->alt);
    fModel.insert("indicatedSpeed", acs->cas);
    fModel.insert("latitude", (static_cast<double>(acs->lat*M_PI)/180));
    fModel.insert("longitude", (static_cast<double>(acs->lon*M_PI)/180));
    fModel.insert("rollAngle", (static_cast<double>(acs->roll*M_PI)/180));
    fModel.insert("pitchAngle", (static_cast<double>(acs->pitch*M_PI)/180));
    fModel.insert("trueSpeed", acs->Yagl);
    fModel.insert("trueCourse", (static_cast<double>(acs->head*M_PI)/180));
    fModel.insert("climbRate", acs->Vu);
    fModel.insert("trueSpeed", acs->tas);
    fModel.insert("turnKVD", static_cast<float>(acs->rpm+30));
    fModel.insert("attackAngleLeft", static_cast<double>(acs->attack));
    fModel.insert("speedX", acs->Vx);
    fModel.insert("speedY", acs->Vy);
    fModel.insert("speedZ", acs->Vz);
    
    QJsonObject JsonObject;
    QJsonArray textsArray = JsonObject["datadev"].toArray();
    textsArray.append(fModel);
    JsonObject["datadev"] = textsArray;
    //================================================================================
    //ui->textBrowser_2->setText(QJsonDocument(JsonObject).toJson(QJsonDocument::Indented));
    //write_file(JsonObject);
    //================================================================================
    //================================================================================
    //arr_record.append(JsonObject);
    //    js_record["oldData"] = arr_record;
    //================================================================================
    QByteArray sendArr = QJsonDocument(JsonObject).toJson(QJsonDocument::Indented);
    ad->send->broadcastDatagram(sendArr, QHostAddress::LocalHost, port_s);
}

void math_model::send_to_model(QJsonObject &object)
{
    //JoystickData jd;
    QByteArray empty = "a";
    bool check_restart = false;
    check_restart = object.find("data").value().toObject().find("JOYS_Throttle_buttons").value().toObject().find("JOYS_Throttle_button_21").value().toBool();
    if(check_restart == true)
    {
        if(check_enable_model == true)
        {
            ad->send->broadcastDatagram(empty, QHostAddress::LocalHost, port_s_172);
            check_enable_model = false;
            time_to_multiki.start(20);
        }
        else {
            time_to_multiki.stop();
            check_enable_model = true;
            ad->send->broadcastDatagram(empty, QHostAddress::LocalHost, port_s_172);
        }
    }
    
    //    bool bt_22 = object.find("data").value().toObject().find("JOYS_Throttle_buttons").value().toObject().find("JOYS_Throttle_button_22").value().toBool();
    //    bool bt_23 = object.find("data").value().toObject().find("JOYS_Throttle_buttons").value().toObject().find("JOYS_Throttle_button_23").value().toBool();
    //    if (bt_22 == true)
    //    {
    //        jd.flaps = 1;//0;
    //    }else if (bt_23 == true)
    //    {
    //        jd.flaps = 1;//2;
    //    }
    //    jd.breaks =  0;//object.find("data").value().toObject().find("JOYS_Throttle_Axis_5").value().toDouble();
    //    jd.rudder = 0;//object.find("data").value().toObject().find("JOYS_TRudder1_Axis_3").value().toDouble();
    //    jd.aileron = 0;//object.find("data").value().toObject().find("JOYS_Joystick1_Axis_2").value().toDouble();
    //    jd.elevator = 0;//object.find("data").value().toObject().find("JOYS_Joystick1_Axis_1").value().toDouble();
    //    jd.throttle = object.find("data").value().toObject().find("JOYS_Throttle_Axis_3").value().toDouble();
    //    //check_state(jd.elevator, jd.aileron);
    //    ui->textBrowser_3->setText("jd.throttle = " + QString::number(jd.throttle)+"\r\n"
    //                              + "jd.breaks = " + QString::number(jd.breaks) + "\r\n"
    //                              + "jd.flaps = " + QString::number(jd.flaps) + "\r\n"
    //                              + "jd.aileron = " + QString::number(jd.aileron) + "\r\n"
    //                              + "jd.elevator = " + QString::number(jd.elevator) + + "\r\n");
    //    QByteArray ba(reinterpret_cast<char*>(&jd),sizeof(JoystickData));
    //    send->broadcastDatagram(ba, QHostAddress::LocalHost, port_send_to_model);
}

void math_model::fromModelToJSON(QByteArray &datagram)
{
    QJsonObject fModel;
    mData = reinterpret_cast<modelData*>(datagram.data());
    fModel.insert("Nx", mData->Nx);
    fModel.insert("Ny", mData->Ny);
    fModel.insert("Nz", mData->Nz);
    fModel.insert("nz", mData->nz);
    fModel.insert("Vpr", mData->Vpr);
    fModel.insert("Vyb", mData->Vyb);
    fModel.insert("W_X", mData->W_X);
    fModel.insert("W_Y", mData->W_Y);
    fModel.insert("W_Z", mData->W_Z);
    fModel.insert("PsiGM", mData->PsiGM);
    fModel.insert("speedX", mData->speedX);
    fModel.insert("speedY", mData->speedY);
    fModel.insert("speedZ", mData->speedZ);
    fModel.insert("turnKND", mData->turnKND);
    fModel.insert("turnKVD", mData->turnKVD);
    fModel.insert("altitude", mData->altitude);
    fModel.insert("latitude", mData->latitude);
    fModel.insert("pressOil", mData->pressOil);
    fModel.insert("climbRate", mData->climbRate);
    fModel.insert("fuelLevel", mData->fuelLevel);
    fModel.insert("longitude", mData->longitude);
    fModel.insert("pressFuel", mData->pressFuel);
    fModel.insert("rollAngle", mData->rollAngle);
    fModel.insert("roll_korr", mData->roll_korr);
    fModel.insert("temperOil", mData->temperOil);
    fModel.insert("trueSpeed", mData->trueSpeed);
    fModel.insert("vibration", mData->vibration);
    fModel.insert("driftAngle", mData->driftAngle);
    fModel.insert("machNumber", mData->machNumber);
    fModel.insert("magnCourse", mData->magnCourse);
    fModel.insert("pitchAngle", mData->pitchAngle);
    fModel.insert("pitch_korr", mData->pitch_korr);
    fModel.insert("trueCourse", mData->trueCourse);
    fModel.insert("absAltitude", mData->absAltitude);
    fModel.insert("deviationRN", mData->deviationRN);
    fModel.insert("positionRUD", mData->positionRUD);
    fModel.insert("relAltitude", mData->relAltitude);
    fModel.insert("temperGases", mData->temperGases);
    fModel.insert("fullPressure", mData->fullPressure);
    fModel.insert("relAltitude1", mData->relAltitude1);
    fModel.insert("altituteRadio", mData->altituteRadio);
    fModel.insert("deviationCPGO", mData->deviationCPGO);
    fModel.insert("absAltitudeBar", mData->absAltitudeBar);
    fModel.insert("deviationFlaps", mData->deviationFlaps);
    fModel.insert("indicatedSpeed", mData->indicatedSpeed);
    fModel.insert("attackAngleLeft", mData->attackAngleLeft);
    fModel.insert("baroCorrection1", mData->baroCorrection1);
    fModel.insert("dynamicPressure", mData->dynamicPressure);
    fModel.insert("pointerPosition", mData->pointerPosition);
    fModel.insert("attackAngleRight", mData->attackAngleRight);
    fModel.insert("deviationElerons", mData->deviationElerons);
    fModel.insert("brakingTemp", mData->brakingTemperature);
    fModel.insert("outdoorTemp", mData->outdoorTemperature);
    QJsonObject JsonObject;
    QJsonArray textsArray = JsonObject["datadev"].toArray();
    textsArray.append(fModel);
    JsonObject["datadev"] = textsArray;
    //================================================================================
    //ui->textBrowser_2->setText(QJsonDocument(JsonObject).toJson(QJsonDocument::Indented));
    //write_file(JsonObject);
    //================================================================================
    QByteArray sendArr = QJsonDocument(JsonObject).toJson(QJsonDocument::Indented);
    ad->send->broadcastDatagram(sendArr, QHostAddress::Broadcast, port_s);
}


void math_model::on_startReadButton_clicked()
{
    if(!mr->isInitised())
    {
        if(QFile::exists(ui->imputFileLine->text()))
        {
            mr->setReadFileName(ui->imputFileLine->text());
            mr->Initise();

            ui->startReadButton->setText("Please wait");
            ui->startReadButton->setEnabled(0);
            ui->infoLabel->setText("Чтение файла...");
            ui->imputFileLine->setReadOnly(1);
            ui->selectFileButton->setEnabled(0);
        }
        else
        {
            ui->infoLabel->setText("Файл не существует");
        }
    }
    else if(!mr->isRunning())
    {
        mr->Continue();
        ui->startReadButton->setText("Pause");
    }
    else
    {
        mr->Pause();
        ui->startReadButton->setText("Continue");
    }
}

void math_model::onReaded()
{
    ui->startReadButton->setText("Pause");
    ui->startReadButton->setEnabled(1);
    ui->timeSlider->setMaximum(mr->getTickVector()->count());
    ui->timeSlider->setEnabled(1);

    ui->stopReadButton->setEnabled(1);

    if(ui->timeRB->isChecked()){
        ui->timeSpin->setEnabled(1);
        ui->timeJumpButton->setEnabled(1);
        ui->frameSpin->setEnabled(0);
        ui->frameJumpButton->setEnabled(0);
        ui->timeSpin->setTime(mr->getFirstTime());
        ui->firstTimeLabel->setText(QString::number(mr->getFirstTime().hour())+":"+QString::number(mr->getFirstTime().minute())+":"+QString::number(mr->getFirstTime().second()));
        ui->lastTimeLabel->setText(QString::number(mr->getLastTime().hour())+":"+QString::number(mr->getLastTime().minute())+":"+QString::number(mr->getLastTime().second()));
    }
    if(ui->frameRB->isChecked())
    {
        ui->frameSpin->setEnabled(1);
        ui->frameJumpButton->setEnabled(1);
        ui->timeSpin->setEnabled(0);
        ui->timeJumpButton->setEnabled(0);
        ui->frameSpin->setMaximum(mr->getLastFrame());
        ui->frameSpin->setMinimum(mr->getFirstFrame());
        ui->firstTimeLabel->setText(QString::number(mr->getFirstFrame()));
        ui->lastTimeLabel->setText(QString::number(mr->getLastFrame()));
    }
    ui->infoLabel->setText("Воспроизведение файла с ИД: "+mr->getHeader());
}

void math_model::on_stopReadButton_clicked()
{
    if(mr->isInitised()){
        mr->Stop();
        ui->startReadButton->setText("Start");
        ui->timeSlider->setEnabled(0);
        ui->frameSpin->setEnabled(0);
        ui->timeSpin->setEnabled(0);
        ui->frameJumpButton->setEnabled(0);
        ui->timeJumpButton->setEnabled(0);
        ui->stopReadButton->setEnabled(0);
        ui->imputFileLine->setReadOnly(0);
        ui->selectFileButton->setEnabled(1);
        ui->firstTimeLabel->setText("");
        ui->lastTimeLabel->setText("");
        ui->currentTimeLabel->setText("");
        ui->frameSpin->clear();
        ui->timeSpin->clear();
        ui->timeSlider->setValue(0);
        ui->infoLabel->setText("Начните чтение");
    }
}

void math_model::on_timeJumpButton_clicked()
{
    if(mr->isInitised())
        mr->setFrame(mr->getTimeVector()->indexOf(ui->timeSpin->time()));
}

void math_model::on_timeSlider_sliderReleased()
{
    if(mr->isInitised())
        mr->setFrame(ui->timeSlider->value());
}

void math_model::on_selectFileButton_clicked()
{
    QString selectedNewFile=
            QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Выберете файл для чтения"), ui->imputFileLine->text()));
    if(!selectedNewFile.isEmpty()&&QFile::exists(selectedNewFile))
    {
        ui->imputFileLine->setText(selectedNewFile);
    }
}

void math_model::on_timeSpin_editingFinished()
{
    if(mr->isInitised())
    {
        if(ui->timeSpin->time()>mr->getLastTime())
            ui->timeSpin->setTime(mr->getLastTime());
        if(ui->timeSpin->time()<mr->getFirstTime())
            ui->timeSpin->setTime(mr->getFirstTime());
    }
}

void math_model::on_frameRB_toggled(bool checked)
{
    if(mr->isInitised()){
        if(!checked){
            ui->timeSpin->setEnabled(1);
            ui->timeJumpButton->setEnabled(1);
            ui->frameSpin->setEnabled(0);
            ui->frameJumpButton->setEnabled(0);
            ui->timeSpin->setTime(mr->getFirstTime());
            ui->firstTimeLabel->setText(QString::number(mr->getFirstTime().hour())+":"+QString::number(mr->getFirstTime().minute())+":"+QString::number(mr->getFirstTime().second()));
            ui->lastTimeLabel->setText(QString::number(mr->getLastTime().hour())+":"+QString::number(mr->getLastTime().minute())+":"+QString::number(mr->getLastTime().second()));
        }
        if(checked)
        {
            ui->frameSpin->setEnabled(1);
            ui->frameJumpButton->setEnabled(1);
            ui->timeSpin->setEnabled(0);
            ui->timeJumpButton->setEnabled(0);
            ui->frameSpin->setMaximum(mr->getLastFrame());
            ui->frameSpin->setMinimum(mr->getFirstFrame());
            ui->firstTimeLabel->setText(QString::number(mr->getFirstFrame()));
            ui->lastTimeLabel->setText(QString::number(mr->getLastFrame()));
        }
    }
}

void math_model::on_frameJumpButton_clicked()
{
    if(mr->isInitised())
        mr->setFrame(mr->getTickVector()->indexOf(ui->frameSpin->value()));
}

void math_model::on_pushButton_clicked()
{
        ad->ResetApp();
}

void math_model::startBroadcast(){
    initialize();
    ad->startBroadcasting();
}

void math_model::on_checkBox_toggled(bool checked)
{
    ui->textBrowser->setEnabled(checked);
}

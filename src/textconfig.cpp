#include "textconfig.h"

textConfig::textConfig(QWidget* parent) : QTextEdit{parent}
{

}

void textConfig::dropEvent(QDropEvent *event){

    //Get data
    const QMimeData* data = event->mimeData();

    //Check if has urls
    if(data->hasUrls()){

        //Use the first url as file
        QFile fin(data->urls()[0].toLocalFile());

        //Check if the file exists
        if(fin.exists()){

            //Open it
            if(fin.open(QIODevice::ReadOnly | QIODevice::Text)){
                QString finalText;
                //Try to read file and store only lines which begins with "geometry/"
                while(!fin.atEnd()){

                    QString line(fin.readLine());
                    std::string lineString = line.toStdString();
                    if(lineString.find("geometry/") == 0){
                        finalText.append(lineString.substr(9).c_str());
                    }
                }
                //Close file
                fin.close();

                //Check if the string is empty
                if(finalText.isEmpty()){
                    //Read again the file but store all lines
                    if(fin.open(QIODevice::ReadOnly | QIODevice::Text)){
                        finalText = QString(fin.readAll());
                        //Close file
                        fin.close();
                    }
                }

                //Print data
                setText(finalText);
            }
        }

    }
    printf("%s\n",data->text().toStdString().c_str());
    fflush(stdout);
}

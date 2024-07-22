#include "ServerHandler.h"

ServerHandler::ServerHandler(int Id,QObject *parent)
    : QThread{parent},ID{Id}
{
}


void ServerHandler::run()
{
    qDebug()<<"Client "<<ID<<" :is running on thread :"<<QThread::currentThreadId();

    socket_ptr =new QTcpSocket();
    socket_ptr->setSocketDescriptor(ID);

    connect(socket_ptr,&QTcpSocket::readyRead,this,&ServerHandler::onReadWrite_slot,Qt::DirectConnection);
    connect(socket_ptr,&QTcpSocket::disconnected,this,&ServerHandler::onDisconnect_slot,Qt::DirectConnection);

    sendMessageToClient("Hello Client Server is greeting...");
    exec();

}


void ServerHandler::sendMessageToClient(QString message)
{
    if(socket_ptr->isOpen())
    {
        QJsonObject jsonObj;
        jsonObj["file_type"] ="Message";
        jsonObj["file_size"] = message.toUtf8().size();
        jsonObj["file_data"] =message;

        QByteArray byteArr = QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);
        QString header = QString("JSonSize:%1/").arg(byteArr.size());

        byteArr.prepend(header.toUtf8());

        socket_ptr->write(byteArr);
    }
}

void ServerHandler::Operations(QString operation, QString bal, QString num)
{
    int pos = operation.indexOf('+');
    int min = operation.indexOf('-');
    QString totalStr;

    if (pos != -1) {
        // Perform addition operation
        QString strTotal = operation.mid(pos + 1);
        int total = strTotal.toInt() + bal.toInt();
        totalStr = QString::number(total);

        // Read JSON data from file
        QJsonDocument jsonDoc = readJsonFileDB();


        // Update balance in JSON data
        QJsonObject jsonObj = jsonDoc.object();
        if (jsonObj.contains(num)) {
            QJsonObject userData = jsonObj[num].toObject();
            userData["balance"] = totalStr;

            //update transactionhistory in json file with current date and timee
            // Update transaction history
            QDateTime currentDateTime = QDateTime::currentDateTime();
            QString formattedDateTime = currentDateTime.toString(Qt::ISODate);
            QJsonArray transactionHistory = userData["transactionHistory"].toArray();
            QJsonObject transactionEntry;
            transactionEntry["date"] = formattedDateTime;
            transactionEntry["amount"] = '+'+strTotal; // Assuming strTotal is the transaction amount
            transactionHistory.append(transactionEntry);
            userData["transactionHistory"] = transactionHistory;
            jsonObj[num] = userData;

            // Save updated JSON data back to file
            if (writeJsonToFileDB(jsonObj)) {
                sendMessageToClient(balance(num));
            } else {
                qDebug() << "Failed to save JSON file.";
                sendMessageToClient("Failed to save JSON file");
            }
        } else {
            qDebug() << "Account number" << num << "not found in JSON.";
            sendMessageToClient("Account number not found");
        }
    } else if ( min != -1) {
        // Perform subtraction operation
        QString strTotal = operation.mid(min + 1);
        int total = bal.toInt() - strTotal.toInt();
        if (total < 0) {
            sendMessageToClient("Not enough balance");
            return;
        }
        totalStr = QString::number(total);

        // Read JSON data from file
        QJsonDocument jsonDoc = readJsonFileDB();

        // Update balance in JSON data
        QJsonObject jsonObj = jsonDoc.object();
        if (jsonObj.contains(num)) {
            QJsonObject userData = jsonObj[num].toObject();
            userData["balance"] = totalStr;


            //update transactionhistory in json file with current date and timee
            // Update transaction history
            QDateTime currentDateTime = QDateTime::currentDateTime();
            QString formattedDateTime = currentDateTime.toString(Qt::ISODate);
            QJsonArray transactionHistory = userData["transactionHistory"].toArray();
            QJsonObject transactionEntry;
            transactionEntry["date"] = formattedDateTime;
            transactionEntry["amount"] = '-'+strTotal; // Assuming strTotal is the transaction amount
            transactionHistory.append(transactionEntry);
            userData["transactionHistory"] = transactionHistory;
            jsonObj[num] = userData;
            // Save updated JSON data back to file
            if (writeJsonToFileDB(jsonObj)) {
                sendMessageToClient(balance(num));
            } else {
                qDebug() << "Failed to save JSON file.";
                sendMessageToClient("Failed to save JSON file");
            }
        } else {
            qDebug() << "Account number" << num << "not found in JSON.";
            sendMessageToClient("Account number not found");
        }
    } else {
        // Invalid operation format
        qDebug() << "Invalid operation format.";
        sendMessageToClient("Invalid operation format");
    }
}
bool ServerHandler::writeJsonToFileDB(const QJsonObject& jsonObj)
{
    QString path = "E:/Client/Client/bankDB.json";
    QFile jsonFile(path);

    if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qDebug() << "Failed to open JSON file for writing:" << jsonFile.errorString();
        return false;
    }

    QJsonDocument jsonDoc(jsonObj);
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();

    qDebug() << "JSON file saved successfully.";
    return true;
}

void ServerHandler::displayDB()
{
    // Read JSON data from file
    QJsonDocument jsonDoc =readJsonFileDB();


    // Check if reading JSON file was successful
    if (jsonDoc.isNull()) {
        qDebug() << "Failed to read JSON file for display.";
        sendMessageToClient("Failed to read JSON file for display");
        return;
    }

    // Convert JSON document to string
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Indented);
    QString jsonString(jsonData);

    // Send JSON string to client
    sendMessageToClient(jsonString);
}



QJsonDocument ServerHandler::readJsonFile()
{  QString path="E:/Client/Client/login.json";
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open JSON file:" << file.errorString();
        return QJsonDocument();
    }

    QByteArray jsonData = file.readAll();
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse JSON:" << parseError.errorString();
        return QJsonDocument();
    }

    return jsonDoc;
}

QJsonDocument ServerHandler::readJsonFileDB()
{  QString path="E:/Client/Client/bankDB.json";
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open JSON file:" << file.errorString();
        return QJsonDocument();
    }

    QByteArray jsonData = file.readAll();
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse JSON:" << parseError.errorString();
        return QJsonDocument();
    }

    return jsonDoc;
}

QString ServerHandler::accnum(QString str)
{  //#ali1234
    QJsonDocument jsonDoc=readJsonFile();
    QJsonObject jsonObj = jsonDoc.object();
    QString username;
    if(str[0]=='*')
    {
        // Input starts with '*', extract username until a digit is encountered
        int startIndex = 1; // Start after '*'
        while (startIndex < str.length() && !str[startIndex].isDigit()) {
            username += str[startIndex];
            ++startIndex;
        }
    }
    else
    {
       username = str.mid(1, str.length() - 5); // Extracts "username" from "*username1234+2000"
    }

    if (jsonObj.contains("users")) {
        QJsonArray usersArray = jsonObj["users"].toArray();
        for (const auto &userObj : usersArray) {
            QJsonObject user = userObj.toObject();
            if (user.contains(username)) {
                QJsonObject userData = user[username].toObject();
               QString nm= userData["accountnumber"].toString();
                return nm;
            }
        }
    }
    // If username is not found
    qDebug() << "User" << username << "not found.";
    return ""; // Return an empty string or handle accordingly

}

QString ServerHandler::balance(QString str)
{
    QJsonDocument jsonDoc = readJsonFileDB();
    QJsonObject jsonObj = jsonDoc.object();

    // Iterate through the keys (account numbers) in the JSON object
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        QString accountNumber = it.key(); // Get the account number (key)
        QJsonObject userData = it.value().toObject();

        // Check if the current account number matches the input str
        if (accountNumber == str) {
            QString balance = userData["balance"].toString();
            qDebug() << "Balance for account number" << str << "is" << balance;
            return balance;
        }
    }

    // If account number is not found
    qDebug() << "Account number" << str << "not found.";
    return ""; // Return an empty string or handle accordingly
}
QString ServerHandler::transac(QString str)
{
    QJsonDocument jsonDoc = readJsonFileDB();
    QJsonObject jsonObj = jsonDoc.object();

    // Check if the account number exists in the JSON object
    if (jsonObj.contains(str)) {
        QJsonArray transactionHistory = jsonObj[str].toObject()["transactionHistory"].toArray();

        QString transactions;
        // Iterate through each transaction record
        for (const auto &transaction : transactionHistory) {
            QJsonObject transactionObj = transaction.toObject();
            QString date = transactionObj["date"].toString();
            QString details = transactionObj["amount"].toString();

            // Format each transaction entry
            QString transactionEntry = QString("%1: %2").arg(date).arg(details);
            transactions += transactionEntry + "\n";
        }

        qDebug() << "Transaction history for account number" << str << ":\n" << transactions;
        return transactions;
    }

    // If account number is not found
    qDebug() << "Account number" << str << "not found.";
    return ""; // Return an empty string or handle accordingly
}
bool ServerHandler::deleteUser(const QString &accountNumber)
{
    // Read JSON data from file
     QJsonDocument jsonDoc = readJsonFileDB();
    if (jsonDoc.isNull()) {
        qDebug() << "Failed to read JSON file.";
        return false;
    }

    // Modify JSON data to delete user based on account number
    QJsonObject jsonObj = jsonDoc.object();
    if (jsonObj.contains(accountNumber)) {
        jsonObj.remove(accountNumber);

        // Save updated JSON data back to file
        if (writeJsonToFileDB(jsonObj)) {
            qDebug() << "User with account number" << accountNumber << "deleted successfully.";
            return true;
        } else {
            qDebug() << "Failed to save JSON file after deleting user.";
            return false;
        }
    } else {
        qDebug() << "Account number" << accountNumber << "not found in JSON.";
        return false;
    }
}
bool ServerHandler::writeJsonToFile(const QJsonObject& jsonObj)
{
    QString path = "E:/Client/Client/login.json";
    QFile jsonFile(path);

    if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qDebug() << "Failed to open JSON file for writing:" << jsonFile.errorString();
        return false;
    }

    QJsonDocument jsonDoc(jsonObj);
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();

    qDebug() << "JSON file saved successfully.";
    return true;
}
bool ServerHandler::deleteUserLogin(const QString &accountNumber)
{
    // Read combined JSON data from file
    QJsonDocument jsonDoc = readJsonFile();
    if (jsonDoc.isNull()) {
        qDebug() << "Failed to read combined JSON file.";
        return false;
    }

    // Modify JSON data to delete user based on account number in 'users' section
    QJsonObject jsonObj = jsonDoc.object();
    if (jsonObj.contains("users")) {
        QJsonArray usersArray = jsonObj["users"].toArray();
        bool userDeleted = false;

        // Iterate through 'users' array to find and delete user
        for (int i = 0; i < usersArray.size(); ++i) {
            QJsonObject userObj = usersArray[i].toObject();
            QString userName = userObj.keys().first(); // Assuming each user has only one key (username)

            QJsonObject userData = userObj[userName].toObject();
            QString userAccountNumber = userData["accountnumber"].toString();

            if (userAccountNumber == accountNumber) {
                usersArray.removeAt(i);
                userDeleted = true;
                break;
            }
        }

        if (!userDeleted) {
            qDebug() << "User with account number" << accountNumber << "not found in 'users' section.";
            return false;
        }

        // Update 'users' array in jsonObj
        jsonObj["users"] = usersArray;

        // Save updated JSON data back to file
        if (writeJsonToFile(jsonObj)) {
            qDebug() << "User with account number" << accountNumber << "deleted successfully from 'users' section.";
            return true;
        } else {
            qDebug() << "Failed to save combined JSON file after deleting user.";
            return false;
        }
    } else {
        qDebug() << "'users' section not found in combined JSON.";
        return false;
    }
}

void ServerHandler::createUser(const QString &userDetails)
{
    // Split the user details string into individual components
    QStringList detailsList = userDetails.split(':');
    if (detailsList.size() != 7) {
        qDebug() << "Invalid user details format.";
        sendMessageToClient("Invalid user details format");
        return;
    }

    QString username = detailsList[1];
    QString password = detailsList[2];
    QString accountNumber = detailsList[3];
    QString age = detailsList[4];
    QString fullname = detailsList[5];
    QString balance = detailsList[6];

    // Read the existing bank database JSON file
    QJsonDocument bankDoc = readJsonFileDB();
    QJsonObject bankObj = bankDoc.object();

    // Check if the account number already exists
    if (bankObj.contains(accountNumber)) {
        qDebug() << "Account number" << accountNumber << "already exists.";
        sendMessageToClient("Account number already exists");
        return;
    }

    // Create a new user object with the provided details
    QJsonObject newUser;
    newUser["username"] = username;
    newUser["age"] = age;
    newUser["balance"] = balance;
    newUser["fullname"] = fullname;
    newUser["transactionHistory"] = QJsonArray(); // Initialize with an empty transaction history

    // Add the new user to the bank database
    bankObj[accountNumber] = newUser;

    // Save the updated bank database back to the JSON file
    if (!writeJsonToFileDB(bankObj)) {
        qDebug() << "Failed to save the updated bank database.";
        sendMessageToClient("Failed to save the updated bank database");
        return;
    }

    // Read the existing login JSON file
    QJsonDocument loginDoc = readJsonFile();
    QJsonObject loginObj = loginDoc.object();

    // Add the new user login details
    QJsonArray usersArray = loginObj["users"].toArray();
    QJsonObject newUserLogin;
    newUserLogin[username] = QJsonObject{
        {"accountnumber", accountNumber},
        {"password", password}
    };
    usersArray.append(newUserLogin);
    loginObj["users"] = usersArray;

    // Save the updated login JSON back to the file
    if (!writeJsonToFile(loginObj)) {
        qDebug() << "Failed to save the updated login data.";
        sendMessageToClient("Failed to save the updated login data");
        return;
    }

    qDebug() << "New user" << fullname << "with account number" << accountNumber << "created successfully.";
    sendMessageToClient("User created successfully");
}
void ServerHandler::updateUser(const QString &input)
{
    // Parse the input string
    QStringList parts = input.split(':');
    if (parts.size() != 7) {
        qDebug() << "Invalid input format";
        sendMessageToClient("Invalid input format");
        return;
    }

    QString username = parts[1];
    QString password = parts[2];
    QString accountNumber = parts[3];
    QString age = parts[4];
    QString fullName = parts[5];
    QString balance = parts[6];

    // Read JSON data from the login file
    QJsonDocument loginDoc = readJsonFile();
    if (loginDoc.isNull()) {
        qDebug() << "Failed to read login JSON file.";
        sendMessageToClient("Failed to read login JSON file.");
        return;
    }

    // Read JSON data from the bankDB file
    QJsonDocument bankDoc = readJsonFileDB();
    if (bankDoc.isNull()) {
        qDebug() << "Failed to read bankDB JSON file.";
        sendMessageToClient("Failed to read bankDB JSON file.");
        return;
    }

    QJsonObject loginObj = loginDoc.object();
    QJsonObject bankObj = bankDoc.object();

    // Find the user in the login JSON data
    if (loginObj.contains("users")) {
        QJsonArray usersArray = loginObj["users"].toArray();
        bool userFound = false;

        for (int i = 0; i < usersArray.size(); ++i) {
            QJsonObject userObj = usersArray[i].toObject();
            if (userObj.contains(username)) {
                QJsonObject userData = userObj[username].toObject();

                // Update only the changed fields
                if (!password.isEmpty() && userData["password"].toString() != password) {
                    userData["password"] = password;
                }



                userObj[username] = userData;
                usersArray[i] = userObj;
                userFound = true;
                break;
            }
        }

        if (!userFound) {
            qDebug() << "User" << username << "not found in login JSON.";
            sendMessageToClient("User not found in login JSON.");
            return;
        }

        loginObj["users"] = usersArray;
    } else {
        qDebug() << "'users' section not found in login JSON.";
        sendMessageToClient("'users' section not found in login JSON.");
        return;
    }

    // Find the user in the bankDB JSON data
    if (bankObj.contains(accountNumber)) {
        QJsonObject bankUserData = bankObj[accountNumber].toObject();

        // Update only the changed fields
        if (!balance.isEmpty() && bankUserData["balance"].toString() != balance) {
            bankUserData["balance"] = balance;
        }
        if (!fullName.isEmpty() && bankUserData["fullname"].toString() != fullName) {
            bankUserData["fullname"] = fullName;
        }
        if (!age.isEmpty() && bankUserData["age"].toString() != age) {
            bankUserData["age"] = age;
        }
        bankObj[accountNumber] = bankUserData;
    } else {
        qDebug() << "Account number" << accountNumber << "not found in bankDB JSON.";
        sendMessageToClient("Account number not found in bankDB JSON.");
        return;
    }

    // Save updated JSON data back to the files
    if (writeJsonToFile(loginObj) && writeJsonToFileDB(bankObj)) {
        sendMessageToClient("User updated successfully");
    } else {
        sendMessageToClient("Failed to save JSON files after updating user.");
    }
}


// JSonSize:JsonObjsize/{"file_data":message,"fie_size":Messagesize,"file_type":"Message"}
void ServerHandler::onReadWrite_slot()
{
    QByteArray byteArr = socket_ptr->readAll();
    QString str = QString(byteArr);
    if(str[0]=='#' || str[0]=='$')    //get acc num for user or admin
    {
       QString num= accnum(str);
        if(num=="")
       {
           sendMessageToClient("not found");
       }
        else
        {
            sendMessageToClient(num);
        }
    }
    else if(str[0]=='!')  //get transac history num for user
    {
        QString num= accnum(str);
        if(num=="")
        {
            sendMessageToClient("not found");
        }
        else
        {
            sendMessageToClient(transac(num));

        }
    }
    else if(str[0]=='%')    //get balance num for user
    {
        QString num= accnum(str);
        if(num=="")
        {
            sendMessageToClient("not found");
        }
        else
        {
            sendMessageToClient(balance(num));

        }
    }
    else if(str[0]=='*') // *ali1234+2000  transaction
    {
        QString num= accnum(str);
        QString bal=balance(num);
        Operations(str,bal,num);
    }
    else if(str[0]=='&')   // &2222 2223 67 transfer
    {
        // Assuming str = "&2222 2223 67 transfer"

        QString num = str.mid(5, 4); // Recipient ID (should be "2223")
        QString myId = str.mid(1, 4); // Your ID (should be "2222")

        QString bal = balance(num); // Assuming this function retrieves balance based on num
        QString amount = str.mid(9); // Amount (should be "67")

        qDebug() << "num:" << num << " myId:" << myId << " amount:" << amount;

        Operations('+' + amount, bal, num); // Perform addition for recipient
        Operations('-' + amount, balance(myId), myId); // Perform subtraction for your account



    }
    else if(str[0]=='~')  //view DB
    {
        displayDB();
    }
    else if(str[0]=='@')   //transaction history for admin
    {
     sendMessageToClient(transac(str.mid(1)));
    }
    else if(str[0]=='^')    //balance for admin
    {
     sendMessageToClient(balance(str.mid(1)));
    }
    else if(str[0]=='=')
    {
        deleteUser(str.mid(1));
        deleteUserLogin(str.mid(1));
    }
    else if(str[0]=='_')
    {
        createUser(str);
    }
    else if(str[0]=='?')
    {
        updateUser(str);
    }

qDebug()<<"Received Data :"<<str<<"From client "<<ID;
}

void ServerHandler::onDisconnect_slot()
{
    if(socket_ptr->isOpen())
    {
        socket_ptr->disconnect();
        qDebug()<<"Client "<<ID<<" has been disconnected";
    }
}

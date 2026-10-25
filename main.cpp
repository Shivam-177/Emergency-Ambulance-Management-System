
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QDialog>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QFrame>
#include <QAbstractItemView>

class AmbulanceApp : public QMainWindow {
    QWidget *central;
    QStackedWidget *pages;

    QLabel *totalLabel;
    QLabel *availableLabel;
    QLabel *activeLabel;
    QLabel *pendingLabel;

    QTableWidget *ambulanceTable;
    QTableWidget *emergencyTable;

public:
    AmbulanceApp() {
        setWindowTitle("Emergency Ambulance Management System");
        resize(1150, 720);

        if (!connectDatabase()) {
            QMessageBox::critical(
                this, "Database Error",
                "Database connection failed. Check Qt SQLite driver."
            );
            return;
        }

        createTables();
        buildUI();
        refreshAll();
    }

private:
    bool connectDatabase() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("ambulance.db");
        return db.open();
    }

    void createTables() {
        QSqlQuery q;

        q.exec(
            "CREATE TABLE IF NOT EXISTS ambulances ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "number TEXT NOT NULL UNIQUE,"
            "driver TEXT NOT NULL,"
            "phone TEXT,"
            "status TEXT NOT NULL DEFAULT 'Available')"
        );

        q.exec(
            "CREATE TABLE IF NOT EXISTS emergencies ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "patient TEXT NOT NULL,"
            "phone TEXT NOT NULL,"
            "location TEXT NOT NULL,"
            "details TEXT,"
            "priority TEXT NOT NULL,"
            "ambulance_id INTEGER DEFAULT 0,"
            "status TEXT NOT NULL DEFAULT 'Pending',"
            "created_at TEXT)"
        );
    }

    QWidget* makePageTitle(const QString &title,
                           const QString &subtitle) {
        QWidget *w = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(w);
        layout->setContentsMargins(0, 0, 0, 12);

        QLabel *heading = new QLabel(title);
        heading->setObjectName("pageTitle");

        QLabel *sub = new QLabel(subtitle);
        sub->setObjectName("subtitle");

        layout->addWidget(heading);
        layout->addWidget(sub);
        return w;
    }

    QPushButton* makeButton(const QString &text,
                            const QString &color = "#2457A7") {
        QPushButton *b = new QPushButton(text);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(
            "QPushButton { background:" + color +
            "; color:white; border:none; border-radius:6px;"
            "padding:10px 14px; font-weight:600; }"
            "QPushButton:hover { background:#173F7A; }"
        );
        return b;
    }

    QWidget* makeCard(const QString &caption,
                      QLabel *&value,
                      const QString &accent) {
        QFrame *card = new QFrame;
        card->setObjectName("card");

        QVBoxLayout *layout = new QVBoxLayout(card);
        QLabel *name = new QLabel(caption);
        name->setObjectName("cardCaption");

        value = new QLabel("0");
        value->setObjectName("cardValue");
        value->setStyleSheet("color:" + accent + ";");

        layout->addWidget(name);
        layout->addWidget(value);
        layout->addStretch();
        return card;
    }

    void buildUI() {
        central = new QWidget;
        setCentralWidget(central);

        QHBoxLayout *root = new QHBoxLayout(central);
        root->setContentsMargins(0, 0, 0, 0);
        root->setSpacing(0);

        // Left navigation sidebar
        QWidget *sidebar = new QWidget;
        sidebar->setObjectName("sidebar");
        sidebar->setFixedWidth(235);

        QVBoxLayout *nav = new QVBoxLayout(sidebar);
        nav->setContentsMargins(16, 22, 16, 18);
        nav->setSpacing(10);

        QLabel *logo = new QLabel("  EMS  |  Dispatch");
        logo->setObjectName("logo");
        nav->addWidget(logo);

        QLabel *menuTitle = new QLabel("MAIN MENU");
        menuTitle->setObjectName("menuTitle");
        nav->addWidget(menuTitle);
        nav->addSpacing(10);

        pages = new QStackedWidget;

        QPushButton *dashboardBtn = makeButton("Dashboard");
        QPushButton *ambulanceBtn = makeButton("Manage Ambulances");
        QPushButton *emergencyBtn = makeButton("Emergency Requests");
        QPushButton *reportsBtn = makeButton("Reports");
        QPushButton *exitBtn = makeButton("Exit", "#475569");

        nav->addWidget(dashboardBtn);
        nav->addWidget(ambulanceBtn);
        nav->addWidget(emergencyBtn);
        nav->addWidget(reportsBtn);
        nav->addStretch();
        nav->addWidget(exitBtn);

        root->addWidget(sidebar);
        root->addWidget(pages, 1);

        QWidget *dashboard = buildDashboard();
        QWidget *ambulances = buildAmbulancePage();
        QWidget *emergencies = buildEmergencyPage();
        QWidget *reports = buildReportsPage();

        pages->addWidget(dashboard);
        pages->addWidget(ambulances);
        pages->addWidget(emergencies);
        pages->addWidget(reports);

        connect(dashboardBtn, &QPushButton::clicked,
                this, [this]() {
            pages->setCurrentIndex(0);
            refreshAll();
        });

        connect(ambulanceBtn, &QPushButton::clicked,
                this, [this]() {
            pages->setCurrentIndex(1);
            refreshAll();
        });

        connect(emergencyBtn, &QPushButton::clicked,
                this, [this]() {
            pages->setCurrentIndex(2);
            refreshAll();
        });

        connect(reportsBtn, &QPushButton::clicked,
                this, [this]() {
            pages->setCurrentIndex(3);
            refreshAll();
        });

        connect(exitBtn, &QPushButton::clicked,
                this, [this]() {
            close();
        });

        setStyleSheet(R"(
            QMainWindow, QWidget {
                background: #F4F7FB;
                color: #1E293B;
                font-family: "Segoe UI";
                font-size: 13px;
            }
            QWidget#sidebar {
                background: #17365D;
            }
            QLabel#logo {
                color: white;
                font-size: 19px;
                font-weight: bold;
                padding: 8px 0 22px 0;
            }
            QLabel#menuTitle {
                color: #B9CBE0;
                font-size: 11px;
                font-weight: bold;
            }
            QLabel#pageTitle {
                font-size: 25px;
                font-weight: bold;
                color: #17365D;
            }
            QLabel#subtitle {
                color: #64748B;
                font-size: 13px;
            }
            QFrame#card {
                background: white;
                border: 1px solid #E2E8F0;
                border-radius: 10px;
            }
            QLabel#cardCaption {
                color: #64748B;
                font-size: 13px;
            }
            QLabel#cardValue {
                font-size: 30px;
                font-weight: bold;
            }
            QTableWidget {
                background: white;
                border: 1px solid #E2E8F0;
                border-radius: 7px;
                gridline-color: #E9EEF5;
                selection-background-color: #DCEBFF;
                selection-color: #17365D;
            }
            QHeaderView::section {
                background: #EAF0F8;
                padding: 9px;
                border: none;
                font-weight: bold;
            }
            QLineEdit, QTextEdit, QComboBox {
                background: white;
                border: 1px solid #CBD5E1;
                border-radius: 5px;
                padding: 7px;
            }
        )");
    }

    QWidget* newContentPage(QVBoxLayout *&layout) {
        QWidget *page = new QWidget;
        layout = new QVBoxLayout(page);
        layout->setContentsMargins(25, 22, 25, 22);
        layout->setSpacing(14);
        return page;
    }

    QTableWidget* makeTable(const QStringList &headers) {
        QTableWidget *table = new QTableWidget;
        table->setColumnCount(headers.size());
        table->setHorizontalHeaderLabels(headers);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->verticalHeader()->setVisible(false);
        table->horizontalHeader()->setSectionResizeMode(
            QHeaderView::Stretch
        );
        table->setAlternatingRowColors(true);
        return table;
    }

    QWidget* buildDashboard() {
        QVBoxLayout *layout;
        QWidget *page = newContentPage(layout);

        layout->addWidget(makePageTitle(
            "Dashboard",
            "Emergency service overview and fleet status"
        ));

        QGridLayout *cards = new QGridLayout;
        cards->setSpacing(14);

        cards->addWidget(makeCard(
            "Total Ambulances", totalLabel, "#2457A7"), 0, 0);
        cards->addWidget(makeCard(
            "Available Ambulances", availableLabel, "#16804A"), 0, 1);
        cards->addWidget(makeCard(
            "Active Emergencies", activeLabel, "#D14343"), 1, 0);
        cards->addWidget(makeCard(
            "Pending Requests", pendingLabel, "#B7791F"), 1, 1);

        layout->addLayout(cards);

        QLabel *note = new QLabel(
            "Quick actions: use the navigation menu to register "
            "an ambulance or create an emergency request."
        );
        note->setWordWrap(true);
        note->setStyleSheet(
            "background:white; padding:18px; border-radius:8px;"
        );
        layout->addWidget(note);
        layout->addStretch();

        return page;
    }

    QWidget* buildAmbulancePage() {
        QVBoxLayout *layout;
        QWidget *page = newContentPage(layout);

        layout->addWidget(makePageTitle(
            "Manage Ambulances",
            "Register vehicles and view their availability"
        ));

        QPushButton *add = makeButton("+ Add Ambulance");
        layout->addWidget(add, 0, Qt::AlignLeft);

        ambulanceTable = makeTable({
            "ID", "Vehicle Number", "Driver", "Phone", "Status"
        });
        layout->addWidget(ambulanceTable, 1);

        connect(add, &QPushButton::clicked,
                this, [this]() {
            addAmbulanceDialog();
        });

        return page;
    }

    QWidget* buildEmergencyPage() {
        QVBoxLayout *layout;
        QWidget *page = newContentPage(layout);

        layout->addWidget(makePageTitle(
            "Emergency Requests",
            "Register a patient and allocate an available ambulance"
        ));

        QHBoxLayout *actions = new QHBoxLayout;
        QPushButton *add = makeButton("+ New Emergency", "#B3261E");
        QPushButton *complete = makeButton("Complete Selected");

        actions->addWidget(add);
        actions->addWidget(complete);
        actions->addStretch();

        layout->addLayout(actions);

        emergencyTable = makeTable({
            "ID", "Patient", "Phone", "Location",
            "Priority", "Ambulance ID", "Status", "Created"
        });
        layout->addWidget(emergencyTable, 1);

        connect(add, &QPushButton::clicked,
                this, [this]() {
            addEmergencyDialog();
        });

        connect(complete, &QPushButton::clicked,
                this, [this]() {
            completeSelectedEmergency();
        });

        return page;
    }

    QWidget* buildReportsPage() {
        QVBoxLayout *layout;
        QWidget *page = newContentPage(layout);

        layout->addWidget(makePageTitle(
            "Reports",
            "Current fleet and emergency request summary"
        ));

        QPushButton *refresh = makeButton("Refresh Report");
        layout->addWidget(refresh, 0, Qt::AlignLeft);

        QLabel *report = new QLabel;
        report->setObjectName("reportText");
        report->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        report->setStyleSheet(
            "background:white; padding:22px; border-radius:8px;"
        );
        layout->addWidget(report, 1);

        connect(refresh, &QPushButton::clicked,
                this, [this, report]() {
            QSqlQuery q;
            QString text;

            q.exec("SELECT status, COUNT(*) FROM ambulances GROUP BY status");
            text += "AMBULANCE SUMMARY\n\n";
            while (q.next()) {
                text += q.value(0).toString() + ": "
                      + q.value(1).toString() + "\n";
            }

            q.exec("SELECT status, COUNT(*) FROM emergencies GROUP BY status");
            text += "\nEMERGENCY SUMMARY\n\n";
            while (q.next()) {
                text += q.value(0).toString() + ": "
                      + q.value(1).toString() + "\n";
            }

            report->setText(text);
        });

        return page;
    }

    void addAmbulanceDialog() {
        QDialog dialog(this);
        dialog.setWindowTitle("Register Ambulance");
        dialog.resize(400, 280);

        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        QFormLayout *form = new QFormLayout;

        QLineEdit *number = new QLineEdit;
        QLineEdit *driver = new QLineEdit;
        QLineEdit *phone = new QLineEdit;

        number->setPlaceholderText("e.g. MH01AB1234");
        driver->setPlaceholderText("Driver full name");
        phone->setPlaceholderText("Contact number");

        form->addRow("Vehicle Number:", number);
        form->addRow("Driver Name:", driver);
        form->addRow("Phone:", phone);
        layout->addLayout(form);

        QPushButton *save = makeButton("Save Ambulance");
        layout->addWidget(save);

        connect(save, &QPushButton::clicked, &dialog,
                [&]() {
            if (number->text().trimmed().isEmpty() ||
                driver->text().trimmed().isEmpty()) {
                QMessageBox::warning(
                    &dialog, "Missing Details",
                    "Vehicle number and driver name are required."
                );
                return;
            }

            QSqlQuery q;
            q.prepare(
                "INSERT INTO ambulances(number, driver, phone, status) "
                "VALUES(?, ?, ?, 'Available')"
            );
            q.addBindValue(number->text().trimmed().toUpper());
            q.addBindValue(driver->text().trimmed());
            q.addBindValue(phone->text().trimmed());

            if (!q.exec()) {
                QMessageBox::warning(
                    &dialog, "Could Not Save",
                    "Could not save ambulance. Check for a duplicate "
                    "vehicle number.\n" + q.lastError().text()
                );
                return;
            }

            dialog.accept();
        });

        if (dialog.exec() == QDialog::Accepted)
            refreshAll();
    }

    void addEmergencyDialog() {
        QDialog dialog(this);
        dialog.setWindowTitle("Register Emergency");
        dialog.resize(440, 430);

        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        QFormLayout *form = new QFormLayout;

        QLineEdit *patient = new QLineEdit;
        QLineEdit *phone = new QLineEdit;
        QLineEdit *location = new QLineEdit;
        QTextEdit *details = new QTextEdit;
        details->setMaximumHeight(80);

        QComboBox *priority = new QComboBox;
        priority->addItems({"Critical", "High", "Normal"});

        form->addRow("Patient Name:", patient);
        form->addRow("Contact Number:", phone);
        form->addRow("Location:", location);
        form->addRow("Emergency Details:", details);
        form->addRow("Priority:", priority);

        layout->addLayout(form);

        QPushButton *save = makeButton("Register Request", "#B3261E");
        layout->addWidget(save);

        connect(save, &QPushButton::clicked, &dialog,
                [&]() {
            if (patient->text().trimmed().isEmpty() ||
                phone->text().trimmed().isEmpty() ||
                location->text().trimmed().isEmpty()) {
                QMessageBox::warning(
                    &dialog, "Missing Details",
                    "Patient, contact and location are required."
                );
                return;
            }

            QSqlDatabase::database().transaction();

            int ambulanceId = 0;
            QString status = "Pending";

            QSqlQuery find;
            find.prepare(
                "SELECT id FROM ambulances "
                "WHERE status='Available' "
                "ORDER BY id LIMIT 1"
            );

            if (!find.exec()) {
                QSqlDatabase::database().rollback();
                QMessageBox::critical(
                    &dialog, "Database Error", find.lastError().text()
                );
                return;
            }

            if (find.next()) {
                ambulanceId = find.value(0).toInt();

                QSqlQuery update;
                update.prepare(
                    "UPDATE ambulances SET status='Busy' "
                    "WHERE id=? AND status='Available'"
                );
                update.addBindValue(ambulanceId);

                if (!update.exec() || update.numRowsAffected() != 1) {
                    QSqlDatabase::database().rollback();
                    QMessageBox::warning(
                        &dialog, "Allocation Error",
                        "Could not allocate the ambulance. Try again."
                    );
                    return;
                }

                status = "Assigned";
            }

            QSqlQuery q;
            q.prepare(
                "INSERT INTO emergencies "
                "(patient, phone, location, details, priority, "
                "ambulance_id, status, created_at) "
                "VALUES(?, ?, ?, ?, ?, ?, ?, ?)"
            );
            q.addBindValue(patient->text().trimmed());
            q.addBindValue(phone->text().trimmed());
            q.addBindValue(location->text().trimmed());
            q.addBindValue(details->toPlainText().trimmed());
            q.addBindValue(priority->currentText());
            q.addBindValue(ambulanceId);
            q.addBindValue(status);
            q.addBindValue(
                QDateTime::currentDateTime().toString(Qt::ISODate)
            );

            if (!q.exec()) {
                QSqlDatabase::database().rollback();
                QMessageBox::critical(
                    &dialog, "Database Error", q.lastError().text()
                );
                return;
            }

            if (!QSqlDatabase::database().commit()) {
                QSqlDatabase::database().rollback();
                QMessageBox::critical(
                    &dialog, "Database Error",
                    "Could not commit the request."
                );
                return;
            }

            if (ambulanceId == 0) {
                QMessageBox::information(
                    &dialog, "Request Pending",
                    "Request saved. No ambulance is available."
                );
            } else {
                QMessageBox::information(
                    &dialog, "Ambulance Assigned",
                    "Request saved and ambulance ID " +
                    QString::number(ambulanceId) + " assigned."
                );
            }

            dialog.accept();
        });

        if (dialog.exec() == QDialog::Accepted)
            refreshAll();
    }

    void completeSelectedEmergency() {
        int row = emergencyTable->currentRow();

        if (row < 0) {
            QMessageBox::warning(
                this, "Select Request",
                "Please select an emergency request first."
            );
            return;
        }

        int id = emergencyTable->item(row, 0)->text().toInt();

        QSqlQuery find;
        find.prepare(
            "SELECT ambulance_id, status FROM emergencies WHERE id=?"
        );
        find.addBindValue(id);

        if (!find.exec() || !find.next()) {
            QMessageBox::warning(
                this, "Error", "Could not find this request."
            );
            return;
        }

        if (find.value(1).toString() == "Completed") {
            QMessageBox::information(
                this, "Already Completed",
                "This request has already been completed."
            );
            return;
        }

        int ambulanceId = find.value(0).toInt();

        if (QMessageBox::question(
                this, "Complete Emergency",
                "Mark request #" + QString::number(id) +
                " as completed?"
            ) != QMessageBox::Yes) {
            return;
        }

        QSqlDatabase::database().transaction();

        QSqlQuery update;
        update.prepare(
            "UPDATE emergencies SET status='Completed' WHERE id=?"
        );
        update.addBindValue(id);

        if (!update.exec()) {
            QSqlDatabase::database().rollback();
            QMessageBox::critical(
                this, "Error", update.lastError().text()
            );
            return;
        }

        // Release the ambulance only if no other active request
        // still uses it.
        if (ambulanceId > 0) {
            QSqlQuery active;
            active.prepare(
                "SELECT COUNT(*) FROM emergencies "
                "WHERE ambulance_id=? AND status='Assigned' AND id<>?"
            );
            active.addBindValue(ambulanceId);
            active.addBindValue(id);

            if (!active.exec() || !active.next()) {
                QSqlDatabase::database().rollback();
                QMessageBox::critical(
                    this, "Error", "Could not verify ambulance status."
                );
                return;
            }

            if (active.value(0).toInt() == 0) {
                QSqlQuery release;
                release.prepare(
                    "UPDATE ambulances SET status='Available' WHERE id=?"
                );
                release.addBindValue(ambulanceId);

                if (!release.exec()) {
                    QSqlDatabase::database().rollback();
                    QMessageBox::critical(
                        this, "Error", release.lastError().text()
                    );
                    return;
                }
            }
        }

        if (!QSqlDatabase::database().commit()) {
            QSqlDatabase::database().rollback();
            QMessageBox::critical(
                this, "Error", "Could not save changes."
            );
            return;
        }

        // Allocate the newly available ambulance to the oldest
        // pending emergency.
        QSqlQuery available;
        available.exec(
            "SELECT id FROM ambulances WHERE status='Available' "
            "ORDER BY id LIMIT 1"
        );

        if (available.next()) {
            int availableId = available.value(0).toInt();

            QSqlQuery pending;
            pending.prepare(
                "SELECT id FROM emergencies WHERE status='Pending' "
                "ORDER BY CASE priority "
                "WHEN 'Critical' THEN 1 "
                "WHEN 'High' THEN 2 ELSE 3 END, id LIMIT 1"
            );

            if (pending.exec() && pending.next()) {
                int pendingId = pending.value(0).toInt();

                QSqlDatabase::database().transaction();

                QSqlQuery allocate;
                allocate.prepare(
                    "UPDATE ambulances SET status='Busy' "
                    "WHERE id=? AND status='Available'"
                );
                allocate.addBindValue(availableId);

                if (allocate.exec() && allocate.numRowsAffected() == 1) {
                    QSqlQuery assign;
                    assign.prepare(
                        "UPDATE emergencies SET ambulance_id=?, "
                        "status='Assigned' WHERE id=? AND status='Pending'"
                    );
                    assign.addBindValue(availableId);
                    assign.addBindValue(pendingId);

                    if (!assign.exec() ||
                        assign.numRowsAffected() != 1) {
                        QSqlDatabase::database().rollback();

                        QSqlQuery undo;
                        undo.prepare(
                            "UPDATE ambulances SET status='Available' "
                            "WHERE id=?"
                        );
                        undo.addBindValue(availableId);
                        undo.exec();
                    } else {
                        QSqlDatabase::database().commit();
                    }
                } else {
                    QSqlDatabase::database().rollback();
                }
            }
        }

        refreshAll();
        QMessageBox::information(
            this, "Completed",
            "Emergency status updated successfully."
        );
    }

    void refreshAll() {
        refreshAmbulances();
        refreshEmergencies();
        refreshDashboard();
    }

    void refreshAmbulances() {
        QSqlQuery q("SELECT id, number, driver, phone, status "
                    "FROM ambulances ORDER BY id DESC");

        ambulanceTable->setRowCount(0);

        while (q.next()) {
            int row = ambulanceTable->rowCount();
            ambulanceTable->insertRow(row);

            for (int col = 0; col < 5; ++col)
                ambulanceTable->setItem(
                    row, col,
                    new QTableWidgetItem(q.value(col).toString())
                );
        }
    }

    void refreshEmergencies() {
        QSqlQuery q(
            "SELECT id, patient, phone, location, priority, "
            "ambulance_id, status, created_at "
            "FROM emergencies ORDER BY id DESC"
        );

        emergencyTable->setRowCount(0);

        while (q.next()) {
            int row = emergencyTable->rowCount();
            emergencyTable->insertRow(row);

            for (int col = 0; col < 8; ++col) {
                QString value = q.value(col).toString();

                if (col == 5 && q.value(col).toInt() == 0)
                    value = "Not assigned";

                if (col == 7)
                    value = value.replace("T", " ");

                emergencyTable->setItem(
                    row, col, new QTableWidgetItem(value)
                );
            }
        }
    }

    void refreshDashboard() {
        QSqlQuery q;

        q.exec("SELECT COUNT(*) FROM ambulances");
        if (q.next()) totalLabel->setText(q.value(0).toString());

        q.exec("SELECT COUNT(*) FROM ambulances "
               "WHERE status='Available'");
        if (q.next()) availableLabel->setText(q.value(0).toString());

        q.exec("SELECT COUNT(*) FROM emergencies "
               "WHERE status='Assigned'");
        if (q.next()) activeLabel->setText(q.value(0).toString());

        q.exec("SELECT COUNT(*) FROM emergencies "
               "WHERE status='Pending'");
        if (q.next()) pendingLabel->setText(q.value(0).toString());
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    AmbulanceApp window;
    window.show();

    return app.exec();
}

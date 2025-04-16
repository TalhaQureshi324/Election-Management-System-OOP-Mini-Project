#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class Person {
public:
    string name;
    int age;
    string nationality;
    Person(string personName, int personAge, string personNationality)
        : name(personName), age(personAge), nationality(personNationality) {}
    virtual bool isEligible() = 0;
};

class Voter : public Person {
public:
    string voterID;
    string pollingStationID;
    Voter(string voterName, int voterAge, string voterNationality, string voterId)
        : Person(voterName, voterAge, voterNationality), voterID(voterId), pollingStationID("") {}
    bool isEligible() override
    {
        int votingAge = 18;
        string requiredNationality = "Pakistan";
        return (age >= votingAge && nationality == requiredNationality);
    }

    void changePollingStation(string newStationID)
    {
        pollingStationID = newStationID;
    }
};

class Candidate : public Person {
public:
    string candidateID;
    string party;
    Candidate(string candidateName, int candidateAge, string candidateNationality, string candidateParty, string candidateId)
        : Person(candidateName, candidateAge, candidateNationality), party(candidateParty), candidateID(candidateId) {}

    bool isEligible() override {
        int candidateAgeRequirement = 25;
        string requiredNationality = "Pakistan";
        return (age >= candidateAgeRequirement && nationality == requiredNationality);
    }
};

class Ballot {
public:
    string electionName;
    Candidate** candidates;
    int numCandidates;
    Ballot(string electionName) : electionName(electionName), candidates(nullptr), numCandidates(0) {}
    void addCandidate(Candidate* candidate) {
        try {
            Candidate** newCandidates = new Candidate * [numCandidates + 1];
            for (int i = 0; i < numCandidates; i++) {
                newCandidates[i] = candidates[i];
            }
            newCandidates[numCandidates] = candidate;
            delete[] candidates;
            candidates = newCandidates;
            numCandidates++;
        }
        catch (bad_alloc& ba) {
            cerr << "Bad allocation: " << ba.what() << endl;
        }
    }
    ~Ballot() {
        for (int i = 0; i < numCandidates; i++) {
            delete candidates[i];
        }
        delete[] candidates;
    }
};

class PollingStation {
public:
    string stationID;
    string constituencyName;
    string location;
    int staffCount;
    int equipmentCount;
    bool isSetupComplete;

    PollingStation(string id, string constituency, string loc)
        : stationID(id), constituencyName(constituency), location(loc),
        staffCount(0), equipmentCount(0), isSetupComplete(false) {}

    void assignPersonnel(int count) {
        staffCount = count;
    }

    void assignEquipment(int count) {
        equipmentCount = count;
    }

    void completeSetup() {
        isSetupComplete = true;
    }
    void savePollingStationDataToFile() const {
        ofstream stationFile("polling_stations.txt", ios::app);
        if (stationFile.is_open()) {
            stationFile << "Station ID: " << stationID << " | Constituency: " << constituencyName << " | Location: " << location << endl;
            stationFile.close();
            cout << "Polling station information saved to 'polling_stations.txt'." << endl;
        }
        else {
            cout << "Error: Unable to open the polling station data file." << endl;
        }
    }
    static void displayPollingStationFromFile(const string& filename) {
        ifstream stationFile(filename);
        if (stationFile.is_open()) {
            string line;
            while (getline(stationFile, line)) {
                cout << line << endl;
            }
            stationFile.close();
        }
        else {
            cout << "Error: Unable to open the polling station data file." << endl;
        }
    }
};

class ElectionManagementSystem {
public:
    Voter** voters;
    Ballot** ballots;
    PollingStation* pollingStations;
    int numVoters;
    int numBallots;
    int numPollingStations;
    static int instanceCount;

    ElectionManagementSystem()
        : voters(nullptr), ballots(nullptr), pollingStations(nullptr), numVoters(0), numBallots(0), numPollingStations(0) {
        instanceCount++;
    }

    bool isVoterEligible(const string& voterName, int voterAge, const string& voterNationality) {
        int votingAge = 18;
        string requiredNationality = "Pakistan";

        if (voterAge < 18) {
            cout << "Your age is less than 18 years." << endl;
            return false;
        }

        if (voterAge >= votingAge && voterNationality == requiredNationality) {
            return true;
        }

        return false;
    }

    void registerVoter(string voterName, int voterAge, const string& voterNationality, string voterID, string pollingStationID) {
        try {
            if (isVoterEligible(voterName, voterAge, voterNationality)) {
                Voter** newVoters = new Voter * [numVoters + 1];
                for (int i = 0; i < numVoters; i++) {
                    newVoters[i] = voters[i];
                }
                newVoters[numVoters] = new Voter(voterName, voterAge, voterNationality, voterID);
                newVoters[numVoters]->pollingStationID = pollingStationID;
                delete[] voters;
                voters = newVoters;
                numVoters++;
                cout << "Voter registered successfully." << endl;
                saveVoterDataToFile();
            }
            else {
                cout << "Voter is not eligible to vote." << endl;
            }
        }
        catch (bad_alloc& ba) {
            cerr << "Bad allocation: " << ba.what() << endl;
        }
    }

    void saveVoterDataToFile() {
        ofstream voterFile("voters.txt");
        if (voterFile.is_open()) {
            for (int i = 0; i < numVoters; i++) {
                voterFile << "Name: " << voters[i]->name << " | Voter ID: " << voters[i]->voterID << " | Polling Station: " << voters[i]->pollingStationID << endl;
            }
            voterFile.close();
            cout << "Voter information saved to 'voters.txt'." << endl;
        }
        else {
            cout << "Error: Unable to open the voter data file." << endl;
        }
    }

    void createBallot(string electionName) {
        Ballot** newBallots = new Ballot * [numBallots + 1];
        for (int i = 0; i < numBallots; i++) {
            newBallots[i] = ballots[i];
        }
        newBallots[numBallots] = new Ballot(electionName);
        delete[] ballots;
        ballots = newBallots;
        numBallots++;
    }

    void addCandidateToBallot(int ballotIndex, Candidate* candidate) {
        if (ballotIndex >= 0 && ballotIndex < numBallots) {
            ballots[ballotIndex]->addCandidate(candidate);
        }
    }
    void displayBallots() {
        cout << "Available Ballots:\n";
        for (int i = 0; i < numBallots; i++) {
            cout << i + 1 << ". " << ballots[i]->electionName << endl;
        }
    }
    void displayVoters() {
        cout << "Registered Voters:\n";
        ifstream voterFile("voters.txt");
        if (!voterFile.is_open()) {
            cout << "No voter data found in 'voters.txt'.\n";
            return;
        }
        string line;
        while (getline(voterFile, line)) {
            cout << line << endl;
        }
        voterFile.close();
    }
    void checkVotingArea() {
        for (int i = 0; i < numBallots; i++) {
            Ballot* currentBallot = ballots[i];
            cout << "Checking voting area for Ballot: " << currentBallot->electionName << endl;
            if (currentBallot->numCandidates > 0) {
                cout << "This ballot has candidates." << endl;
            }
            else {
                cout << "This ballot has no candidates. It needs to be prepared." << endl;
            }
        }
    }

    void removePollingStation(const string& stationID) {
        for (int i = 0; i < numPollingStations; i++) {
            if (pollingStations[i].stationID == stationID) {
                // Shift the array elements to remove the polling station
                for (int j = i; j < numPollingStations - 1; j++) {
                    pollingStations[j] = pollingStations[j + 1];
                }
                numPollingStations--;
                cout << "Polling station with ID " << stationID << " removed successfully.\n";
                return;
            }
        }
        cout << "Polling station with ID " << stationID << " not found.\n";
    }

    void generateVotersReport() const {
        cout << "Voters Report:\n";
        cout << "--------------------------------------------\n";

        if (numVoters == 0) {
            cout << "No voters are registered in the system.\n";
            return;
        }
        cout << "Registered Voters:\n";
        for (int i = 0; i < numVoters; i++) {
            const Voter* voter = voters[i];
            cout << "Name: " << voter->name << " | Voter ID: " << voter->voterID << " | Polling Station: " << voter->pollingStationID << endl;
        }
        cout << "--------------------------------------------\n";
        cout << "Total Registered Voters: " << numVoters << endl;
    }

    void fileCandidateNomination(const string& candidateName, const string& candidateParty, const string& candidateID) {
        // Store candidate nomination data in a file
        ofstream candidateFile("candidates.txt", ios::app);
        if (candidateFile.is_open()) {
            candidateFile << "Candidate Name: " << candidateName << " | Candidate Party: " << candidateParty << " | Candidate ID: " << candidateID << endl;
            candidateFile.close();
            cout << "Candidate nomination filed successfully and stored in 'candidates.txt'.\n";
        }
        else {
            cout << "Error: Unable to open the candidate nomination file.\n";
        }
    }

    ~ElectionManagementSystem() {
        for (int i = 0; i < numVoters; i++) {
            delete voters[i];
        }
        delete[] voters;
        for (int i = 0; i < numBallots; i++) {
            delete ballots[i];
        }
        delete[] ballots;
        delete[] pollingStations;
    }
    void displayPollingStationDataFromFile() {
        PollingStation::displayPollingStationFromFile("polling_stations.txt");
    }
};

int ElectionManagementSystem::instanceCount = 0;
void displayMenu() 
{
    cout << "*" << endl;
    cout << "*     Election Management System (EMS)     *" << endl;
    cout << "*" << endl;
    cout << "1. Register Voter" << endl;
    cout << "2. Create Ballot" << endl;
    cout << "3. Add Candidate to Ballot" << endl;
    cout << "4. Display Ballots" << endl;
    cout << "5. Display Registered Voters" << endl;
    cout << "6. Check the Voting Area" << endl;
    cout << "7. Create Polling Station" << endl;
    cout << "8. display Polling Station" << endl;
    cout << "9. Remove Polling Station" << endl;
    cout << "10. Generate Report (Voters)" << endl;
    cout << "11. Candidate Nomination" << endl;
    cout << "12. Generate Report (Admin)" << endl;
    cout << "13. Vote Casting" << endl;
    cout << "0. Exit" << endl;
    cout << "Enter your choice: ";
}
void generateAdminReport(const ElectionManagementSystem& ems) {
    ems.generateVotersReport();

    ifstream candidateFile("candidates.txt");
    if (candidateFile.is_open()) {
        cout << "Candidate Nominations Report:\n";
        cout << "------------------------------------\n";
        string line;
        while (getline(candidateFile, line)) {
            cout << line << endl;
        }
        candidateFile.close();
    }
    else {
        cout << "Error: Unable to open candidate nomination file.\n";
    }

    cout << "Polling Stations Report:\n";
    for (int i = 0; i < ems.numPollingStations; i++) {
        cout << "Station ID: " << ems.pollingStations[i].stationID << " | Constituency: " << ems.pollingStations[i].constituencyName << " | Location: " << ems.pollingStations[i].location << endl;
        cout << "Staff Count: " << ems.pollingStations[i].staffCount << " | Equipment Count: " << ems.pollingStations[i].equipmentCount << " | Setup Complete: " << (ems.pollingStations[i].isSetupComplete ? "Yes" : "No") << endl;
        cout << "------------------------------------\n";
    }

    // Include additional reporting functionalities here for system integrity, user access, activity logs, and performance
    // Based on your EMS's structure and data storage

    // Add any other reporting functionalities as needed
}
void voteCasting(ElectionManagementSystem& ems) {
    // Assuming voters are identified by their voter ID
    string voterID;
    cout << "Enter your Voter ID to cast votes: ";
    cin >> voterID;

    // Find the voter by their ID
    int voterIndex = -1;
    for (int i = 0; i < ems.numVoters; ++i) {
        if (ems.voters[i]->voterID == voterID) {
            voterIndex = i;
            break;
        }
    }

    if (voterIndex != -1) {
        cout << "Vote Casting for " << ems.voters[voterIndex]->name << " (Voter ID: " << voterID << ")" << endl;

        // MNA Vote
        cout << "Select your preferred MNA candidate:" << endl;
        ems.displayBallots(); // Assuming MNA candidates are added to existing ballots
        int mnaBallotIndex;
        cout << "Enter the index of the ballot for MNA candidate: ";
        cin >> mnaBallotIndex;

        // MPA Vote
        cout << "Select your preferred MPA candidate:" << endl;
        ems.displayBallots(); // Assuming MPA candidates are added to existing ballots
        int mpaBallotIndex;
        cout << "Enter the index of the ballot for MPA candidate: ";
        cin >> mpaBallotIndex;

        // Check validity of indices
        if (mnaBallotIndex >= 1 && mnaBallotIndex <= ems.numBallots &&
            mpaBallotIndex >= 1 && mpaBallotIndex <= ems.numBallots) {

            // Record the votes for both MNA and MPA candidates for the voter
            Candidate* selectedMNA = ems.ballots[mnaBallotIndex - 1]->candidates[0]; // Assuming one candidate per ballot
            Candidate* selectedMPA = ems.ballots[mpaBallotIndex - 1]->candidates[0]; // Assuming one candidate per ballot

            // Save vote data to a file or storage system
            ofstream voteData("vote_data.txt", ios::app);
            if (voteData.is_open()) {
                voteData << "Voter ID: " << voterID << " | MNA Vote: " << selectedMNA->name << " | MPA Vote: " << selectedMPA->name << endl;
                voteData.close();
                cout << "Votes recorded successfully.\n";
            }
            else {
                cout << "Error: Unable to save vote data.\n";
            }
        }
        else {
            cout << "Invalid ballot indices.\n";
        }
    }
    else {
        cout << "Voter ID not found.\n";
    }
}

void createBallot(ElectionManagementSystem& ems) {
    string electionName;
    cout << "Enter election name: ";
    cin >> electionName;
    ems.createBallot(electionName);
}

void addCandidateToBallot(ElectionManagementSystem& ems) {
    int ballotIndex;
    cout << "Select a ballot to add a candidate (by index):\n";
    ems.displayBallots();
    cout << "Enter the index of ballot: " << endl;
    cin >> ballotIndex;
    if (ballotIndex >= 1 && ballotIndex <= ems.numBallots) {
        string candidateName, candidateParty;
        cout << "Enter candidate name: ";
        cin.ignore();
        getline(cin, candidateName);
        cout << "Enter candidate party: ";
        getline(cin, candidateParty);
        string candidateID;
        cout << "Enter candidate ID: ";
        cin >> candidateID;
        Candidate* newCandidate = new Candidate(candidateName, 0, "", candidateParty, candidateID);
        ems.addCandidateToBallot(ballotIndex - 1, newCandidate);
        cout << "Candidate added to the ballot.\n";
    }
    else {
        cout << "Invalid ballot index.\n";
    }
}

void createPollingStation(ElectionManagementSystem& ems) {
    string stationID, constituency, location;
    int staffCount, equipmentCount;
    cout << "Enter Polling Station ID: ";
    cin >> stationID;
    cout << "Enter Constituency: ";
    cin.ignore();
    getline(cin, constituency);
    cout << "Enter Location: ";
    getline(cin, location);
    PollingStation newPollingStation(stationID, constituency, location);
    cout << "Enter the number of staff members at the polling station: ";
    cin >> staffCount;
    newPollingStation.assignPersonnel(staffCount);
    cout << "Enter the number of equipment units available at the polling station: ";
    cin >> equipmentCount;
    newPollingStation.assignEquipment(equipmentCount);
    newPollingStation.completeSetup();
    newPollingStation.savePollingStationDataToFile();
    ems.pollingStations[ems.numPollingStations] = newPollingStation;
    ems.numPollingStations++;
    cout << "Polling station created with ID: " << stationID << endl;
}


void registerVoter(ElectionManagementSystem& ems) {
    string name, voterID;
    int age;
    string nationality;
    string pollingStationID;
    cout << "Enter voter name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter voter ID: ";
    cin >> voterID;
    cout << "Enter voter's age: ";
    cin >> age;
    cout << "Enter voter's nationality: ";
    cin >> nationality;
    cout << "Enter polling station ID for the voter: ";
    cin >> pollingStationID;
    ems.registerVoter(name, age, nationality, voterID, pollingStationID);
}
void removePollingStation(ElectionManagementSystem& ems) {
    string stationID;
    cout << "Enter Polling Station ID to remove: ";
    cin >> stationID;
    ems.removePollingStation(stationID);
}

void candidateNomination(ElectionManagementSystem& ems) {
    string candidateName, candidateParty, candidateID;
    cout << "Enter candidate name: ";
    cin.ignore();
    getline(cin, candidateName);
    cout << "Enter candidate party: ";
    getline(cin, candidateParty);
    cout << "Enter candidate ID: ";
    cin >> candidateID;
    ems.fileCandidateNomination(candidateName, candidateParty, candidateID);
}
void candidateNominationMNA(ElectionManagementSystem& ems) {
    string candidateName, candidateParty, candidateID;
    cout << "Enter MNA candidate name: ";
    cin.ignore();
    getline(cin, candidateName);
    cout << "Enter MNA candidate party: ";
    getline(cin, candidateParty);
    cout << "Enter MNA candidate ID: ";
    cin >> candidateID;

    ofstream candidateFile("candidates.txt", ios::app);
    if (candidateFile.is_open()) {
        candidateFile << "MNA Candidate Name: " << candidateName << " | Party: " << candidateParty << " | ID: " << candidateID << endl;
        candidateFile.close();
        cout << "MNA Candidate nomination filed successfully and stored in 'candidates.txt'.\n";
    }
    else {
        cout << "Error: Unable to open the candidate nomination file.\n";
    }
}
void candidateNominationMPA(ElectionManagementSystem& ems) {
    string candidateName, candidateParty, candidateID;
    cout << "Enter MPA candidate name: ";
    cin.ignore();
    getline(cin, candidateName);
    cout << "Enter MPA candidate party: ";
    getline(cin, candidateParty);
    cout << "Enter MPA candidate ID: ";
    cin >> candidateID;

    ofstream candidateFile("candidates.txt", ios::app);
    if (candidateFile.is_open()) {
        candidateFile << "MPA Candidate Name: " << candidateName << " | Party: " << candidateParty << " | ID: " << candidateID << endl;
        candidateFile.close();
        cout << "MPA Candidate nomination filed successfully and stored in 'candidates.txt'.\n";
    }
    else {
        cout << "Error: Unable to open the candidate nomination file.\n";
    }
}
int main() {
    try {
        ElectionManagementSystem ems;
        int choice;

        do {
            displayMenu();
            cin >> choice;
            switch (choice) {
            case 1:
                registerVoter(ems);
                break;
            case 2:
                createBallot(ems);
                break;
            case 3:
                addCandidateToBallot(ems);
                break;
            case 4:
                ems.displayBallots();
                break;
            case 5:
                ems.displayVoters();
                break;
            case 6:
                ems.checkVotingArea();
                break;
            case 7:
                createPollingStation(ems);
                break;
            case 8:
                ems.displayPollingStationDataFromFile();
                break;
            case 9:
                removePollingStation(ems);
                break;
            case 10:
                ems.generateVotersReport();
                break;
            case 11:
                int candidateType;
                cout << "Select candidate type:\n";
                cout << "1. MNA (Member of the National Assembly)\n";
                cout << "2. MPA (Member of the Provincial Assembly)\n";
                cout << "Enter candidate type (1 or 2): ";
                cin >> candidateType;
                if (candidateType == 1) {
                    candidateNominationMNA(ems);
                }
                else if (candidateType == 2) {
                    candidateNominationMPA(ems);
                }
                else {
                    cout << "Invalid candidate type. Please try again." << endl;
                }
                break;
            case 12:
                generateAdminReport(ems);
                break;
            case 13:
                voteCasting(ems);
                break;
            case 0:
                cout << "Exiting EMS. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
            }
        } while (choice != 0);
        cout << "EMS instances created: " << ElectionManagementSystem::instanceCount << endl;
    }
    catch (const exception& e) {
        cerr << "Exception caught: " << e.what() << endl;
    }
    catch (...) {
        cerr << "Unknown exception caught." << endl;
    }
    return 0;
}
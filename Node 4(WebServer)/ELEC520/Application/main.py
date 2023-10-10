# Sources:
# https://pythonprogramming.altervista.org/3939-2/

# Libraries:
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore
import tkinter as tk
from tkinter import ttk
from re import findall
from subprocess import Popen, PIPE
from cryptography.fernet import Fernet
import csv

cred = credentials.Certificate("ossweb-97b03-firebase-adminsdk-ra39j-a588abd756.json")

firebase_admin.initialize_app(cred, {
    'projectId' : "ossweb-97b03"
})

db = firestore.client()

keystorage = db.collection("Key").document("Key")

def Key_Gen():
    key = Fernet.generate_key()

    data = {"Key" : key}

    keystorage.set(data)

def Load_Key():
    result = keystorage.get().to_dict()
    key = result['Key']
    return key


#Key_Gen()

def encrypt(filename, key):
    f = Fernet(key)

    with open(filename, "rb") as file:
        file_data = file.read()
    
    encrypted_data = f.encrypt(file_data)

    with open(filename, "wb") as file:
        file.write(encrypted_data)

def decrypt(filename, key):
    f = Fernet(key)

    with open(filename, "rb") as file:
        encrypted_data = file.read()
    
    decrypted_data = f.decrypt(encrypted_data)

    with open(filename, "wb") as file:
        file.write(decrypted_data)

key = Load_Key()
file = "login.csv"

encrypt(file, key)

login = tk.Tk()
login.geometry("700x400")
username = tk.Label(login, text="Username")
username.pack()
user = tk.Entry(login)
user.pack()
password = tk.Label(login, text="Password")
password.pack()
passwordent = tk.Entry(login, show='*')
passwordent.pack()

loginallowed = 0

def CheckandLogin():
    global loginallowed
    global key, file
    decrypt(file, key)
    usernameresult = user.get()
    passwordresult = passwordent.get()

    with open("login.csv", 'r') as filedata:
        csvreader = filedata.readlines()

    header = csvreader[:1]
    rows = csvreader[1:]

    header = header[0].split(",")[1]
    rows = rows[0].split(",")[1]

    Masterusername = header
    Masterpassword = rows
    
    filedata.close()

    encrypt(file, key)

    if (usernameresult == Masterusername) and (passwordresult == Masterpassword):
        loginallowed = 1
        login.destroy()

submition = tk.Button(login, text="Log In", command=CheckandLogin)
submition.pack()

login.mainloop()

#tkinter setup
root = tk.Tk()
if loginallowed == 0:
    root.destroy()
    quit()

#setup



'''
document_ref = db.collection("NodeInfo").stream()

#BigDict = ()

nodeDocument = 0

for doc in document_ref:
    docdict = doc.to_dict()
    print(docdict)
    BigDict = BigDict + (docdict, )

print(BigDict)
'''

root.geometry("700x400")
root.title("OSS Hub | Open Source Security Hub")

Tabs = ttk.Notebook(root)
    
Dashboard = ttk.Frame(Tabs)
AddNewNode = ttk.Frame(Tabs)
Info = ttk.Frame(Tabs)

Tabs.add(Dashboard, text="Dashboard")
Tabs.add(AddNewNode, text="New Nodes")
Tabs.add(Info, text="Information")
Tabs.pack(expand=1, fill="both")

table = ttk.Treeview(Dashboard)



def DashboardFunction(BigDict) -> None:
    global table
    try:
        for item in table.get_children():
            table.delete(item)
    except:
        pass

    # initialise table
    table["columns"] = ("Node Active", "Node Name", "Node Address", "Node Alarm")

    table.column("#0", width=0, stretch=tk.NO) # Hide first column
    table.column("Node Active", width=100)
    table.column("Node Name", width=100)
    table.column("Node Address", width=100)
    table.column("Node Alarm", width=100)

    # Create headings for each column
    table.heading("Node Active", text="Node Active")
    table.heading("Node Name", text="None Name")
    table.heading("Node Address", text="Node Address")
    table.heading("Node Alarm", text = "Node Alarm")


    for i in range(len(BigDict)):
        #print(BigDict[i]['NodeActive'])
        table.insert("", tk.END, text="", values=(BigDict[i]['NodeActive'], BigDict[i]['NodeName'], BigDict[i]['NodeAddr'], BigDict[i]['NodeAlarm']))
    
    table.pack()

def ping_nodes(IpAddress):
    '''
    Ping the nodes from the Ip address list
    '''

    successful_pings = []
    unsuccessful_pings = []

    ping_count = 1

    for ip in IpAddress:
        data = ""
        output = Popen(f"ping {ip} -n {ping_count}", stdout=PIPE, encoding="utf-8")

        for line in output.stdout:
            data = data + line
            ping_test = findall("TTL", data)

        if ping_test:
            print(f"{ip} : Successful Ping")
            successful_pings.append(ip)
        else:
            print(f"{ip} : Failed Ping")
            unsuccessful_pings.append(ip)


    
    return successful_pings, unsuccessful_pings

def refreshTable() ->None:
    BigDict = ()
    document_ref = db.collection("NodeInfo").stream()

    for doc in document_ref:
        docdict = doc.to_dict()
        print(docdict)
        
        if docdict in BigDict:
            pass
        else:
            BigDict = BigDict + (docdict, )

    Ipaddresses = [d.get('NodeAddr', None) for d in BigDict]

    print(Ipaddresses)
    Successfull_IP, Unsuccessfull_IP = ping_nodes(Ipaddresses)

    for Ip in Unsuccessfull_IP:
        result = next((item for item in BigDict if item["NodeAddr"] == Ip), None)
        Name = result['NodeName']

        Document = db.collection('NodeInfo').document(Name)
        Document.update({'NodeActive': "false"})
    

    DashboardFunction(BigDict)

    Dashboard.after(5000, refreshTable)   


refreshTable()

'''
Refresh = tk.Button(Dashboard, text="Update", command=refreshTable)
Refresh.pack()
'''

NodeName1 = tk.Label(AddNewNode, text="Node Name")
NodeName1.pack()
NodeAnswer = tk.Entry(AddNewNode)
NodeAnswer.pack()

NodeAddr1 = tk.Label(AddNewNode, text="Node Address")
NodeAddr1.pack()
NodeAddrAnswer = tk.Entry(AddNewNode)
NodeAddrAnswer.pack()

NodeActive1 = tk.Label(AddNewNode, text="Active Node?")
NodeActive1.pack()
NodeActiveAnswer = tk.Entry(AddNewNode)
NodeActiveAnswer.pack()

def AddNodeCallback():
    NN = NodeAnswer.get()
    NAd = NodeAddrAnswer.get()
    NAc = NodeActiveAnswer.get()

    data = {'NodeActive': NAc, 'NodeName': NN, 'NodeAddr': NAd}

    db.collection("NodeInfo").document(NN).set(data)
    

SubmitAddNodeBtn = tk.Button(AddNewNode, text="Submit", command=AddNodeCallback)
SubmitAddNodeBtn.pack()

def DeleteNodeCallback():
    NN = NodeAnswer.get()

    try:
        db.collection("NodeInfo").document(NN).delete()
    except:
        print("Unknown Node")

DeleteNodeBtn = tk.Button(AddNewNode, text="Delete Node", command=DeleteNodeCallback)
DeleteNodeBtn.pack()

def UpdateNodesCallback():
    NN = NodeAnswer.get()
    NAd = NodeAddrAnswer.get()
    NAc = NodeActiveAnswer.get()

    reference = db.collection("NodeInfo").document(NN)

    reference.update({'NodeName': NN})
    reference.update({'NodeAddr': NAd})
    reference.update({'NodeActive': NAc})   


UpdateNodeBtn = tk.Button(AddNewNode, text="Update Node", command=UpdateNodesCallback)
UpdateNodeBtn.pack()

def Reset_Key():
    global file, key
    decrypt(file, key)
    key = Key_Gen()
    encrypt(file, key)

ResetKeyBtn = tk.Button(Info, text="RESET KEY", command= Reset_Key)
ResetKeyBtn.pack()


root.mainloop()

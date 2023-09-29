# Sources:
# https://pythonprogramming.altervista.org/3939-2/

# Libraries:
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore
import tkinter as tk
from tkinter import ttk

Masterusername = "FirebaseOssHosting"
Masterpassword = "1234"

login = tk.Tk()
login.geometry("700x400")
username = tk.Label(login, text="Username")
username.pack()
user = tk.Entry(login)
user.pack()
password = tk.Label(login, text="Password")
password.pack()
passwordent = tk.Entry(login)
passwordent.pack()

loginallowed = 0

def CheckandLogin():
    global loginallowed
    usernameresult = user.get()
    passwordresult = passwordent.get()

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
cred = credentials.Certificate("ossweb-97b03-firebase-adminsdk-ra39j-e4f9b8d3cb.json")

firebase_admin.initialize_app(cred, {
    'projectId' : "ossweb-97b03"
})

db = firestore.client()

document_ref = db.collection("NodeInfo").stream()

BigDict = ()

nodeDocument = 0

for doc in document_ref:
    docdict = doc.to_dict()
    print(docdict)
    BigDict = BigDict + (docdict, )

print(BigDict)

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


def DashboardFunction() -> None:
    global table
    try:
        for item in table.get_children():
            table.delete(item)
    except:
        pass

    # initialise table
    table["columns"] = ("Node Active", "Node Name", "Node Address")

    table.column("#0", width=0, stretch=tk.NO) # Hide first column
    table.column("Node Active", width=100)
    table.column("Node Name", width=100, anchor=tk.CENTER)
    table.column("Node Address", width=100)

    # Create headings for each column
    table.heading("Node Active", text="Node Active")
    table.heading("Node Name", text="None Name")
    table.heading("Node Address", text="Node Address")


    for i in range(len(BigDict)):
        #print(BigDict[i]['NodeActive'])
        table.insert("", tk.END, text="", values=(BigDict[i]['NodeActive'], BigDict[i]['NodeName'], BigDict[i]['NodeAddr']))
    
    table.pack()

DashboardFunction()

def refreshTable() ->None:
    global BigDict
    document_ref = db.collection("NodeInfo").stream()

    for doc in document_ref:
        docdict = doc.to_dict()
        print(docdict)
        
        if docdict in BigDict:
            pass
        else:
            BigDict = BigDict + (docdict, )

    
    DashboardFunction()

Refresh = tk.Button(Dashboard, text="Update", command=refreshTable)
Refresh.pack()

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


## Data from sensor


root.mainloop()

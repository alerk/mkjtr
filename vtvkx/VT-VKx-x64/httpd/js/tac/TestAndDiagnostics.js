// this file gets included in page provided by
// TAC http handler /TestAndDiagnostics

// base url
var base_uri = 'TestAndDiagnostics';

// stores
var evals = [];

// helper to get json from TAC kernel
function get(uri) {
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("GET", uri, false);
    xmlHttp.send( null );
    return xmlHttp.responseText;
}

// helper to post json data to TAC kernel
function set(uri, data) {
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("POST", uri, false);
    xmlHttp.send(data);
    return xmlHttp.responseText;
}

/* helper for dropdown box evaluating */
function getSelectedEnumByID(selectID) {
    var select = document.getElementById(selectID);
    var selected = select.options[select.selectedIndex];
    return selected.innerHTML;
}

/* helper for input box evaluating */
function getInputTextByID(inputID) {
    var inputBox = document.getElementById(inputID);
    var integer = parseInt(inputBox.value);
    if (isNaN(integer)) {
        alert(inputID + ": enter integer please");
    }
    return integer;
}

/* generic function to add new row to our table */
function appendRow(name, value) {
    var table = document.getElementById("table");
    var fieldRow = document.createElement("tr");
    var nameCell= document.createElement("td");
    var valueCell= document.createElement("td");
    nameCell.appendChild(name);
    valueCell.appendChild(value);
    fieldRow.appendChild(nameCell);
    fieldRow.appendChild(valueCell);
    table.appendChild(fieldRow);
}

/* clear the table */
function tableClear() {
    document.getElementById("button").innerHTML = '';
    document.getElementById("table").innerHTML = '';
}

/* show button which sends the dialog */
function showCallButton(functionName) {
    var buttonDiv = document.getElementById("button");
    var send = document.createElement("button");
    send.setAttribute("type", "button");
    send.setAttribute("onclick", "sendForm(\"" + functionName + "\")");
    send.innerHTML = "Call";
    buttonDiv.appendChild(send);
}

/* functions for navigation links manipulation */
function setNaviBack(action) {
    setNavigation("back", action);
}

function setNaviRefresh(action) {
    setNavigation("refresh", action);
}

function setNavigation(elemId, action) {
    var navHref = document.getElementById(elemId);
    if (action == "") {
        navHref.style.visibility = "hidden";
    } else {
        navHref.style.visibility = "visible";
        navHref.setAttribute("href", "#");
        navHref.setAttribute("onclick", action);
    }
}

/* this is top level function which is called for both, functions
 * not accepting and accepting argument
 * that happens if you click on function name in the main list */
function showDialog(name) {
    var json = get("?name=" + name);
    var struct = JSON.parse(json);
   /* if GET request returns error we need to make POST with arguments
     * but we need to know the arguments so ve call Function&form=1 to get
     * JSON describing arguments and then we call showSeter which renders
     * html dialog for based on this JSON string */
    if (struct['status'] == "ERROR_PARAM") {
        struct = JSON.parse(get("?name=" + name + "&form=1"));
        showForm(name, struct)
        showCallButton(name);
        setNaviRefresh("");
    } else {
        showForm(name, struct);
        setNaviRefresh("showDialog(\"" + name + "\")");
    }
    setNaviBack("location.href = '/" + base_uri + "'");
}

/* this function is called on JSON array, it recursively
 * call print* on each element of array */
function printArray(prefix, array) {
    evals.push("." + prefix + " = []");
    for (var i = 0; i < array.length; i++) {
        printBase(prefix + "[" + i + "]", array[i]);
    }
}

/* called for number objects (type = __t_number) and renders
 * html input box to enter number */
function printNumber(prefix, enums) {
    var name = document.createTextNode(prefix);
    var inputBox = document.createElement("input");
    inputBox.setAttribute("type", "text");
    inputBox.setAttribute("id", prefix);
    inputBox.value = "0";
    appendRow(name, inputBox);
    evals.push("." + prefix + " = getInputTextByID(\"" + prefix + "\")");
}

/* called for enums objects (type = __t_enums) and renders html
 * drop down HTML box with these enums */
function printEnums(prefix, enums) {
    var enumVals = enums["enums"];
    var name = document.createTextNode(prefix);
    var select = document.createElement("select");
    select.setAttribute("id", prefix);
    for (var i = 0; i < enumVals.length; i++) {
        var opt = document.createElement("option");
        opt.innerHTML = enumVals[i];
        select.appendChild(opt);
    }
    appendRow(name, select);
    evals.push("." + prefix + " = getSelectedEnumByID(\"" + prefix + "\")");
}

/* called for plain text (usually called for getters) */
function printRow(prefix, str) {
    appendRow(document.createTextNode(prefix), document.createTextNode(str));
}

/* this function is called on each JSON entity and it simply
 * redirects the call based on entity type */
function printBase(prefix, struct) {
    if (Object.prototype.toString.call(struct) == '[object Array]') {
        printArray(prefix, struct);
    } else if (typeof(struct) === "object") {
        if (struct["type"] === "__t_number") {
            printNumber(prefix, struct);
        } else if (struct["type"] === "__t_enum") {
            printEnums(prefix, struct);
        } else {
            printObject(prefix, struct);
        }
    } else {
        printRow(prefix, struct);
    }
}

/* this is called for all JSON objects, it recuresivelly calls
 * print* function for all members of object */
function printObject(prefix, struct) {
    var dot = "";
    if (prefix) {
        dot = ".";
    }
    evals.push(dot + prefix + " = {}");
    for (var k in struct) {
        var p = prefix + dot + k;
        printBase(p, struct[k]);
    }
}

/* show JSON structure from TAC as a html table */
function showForm(name, struct) {
    tableClear();
    document.title = base_uri + " - " + name;
    var table = document.getElementById("table");
    var title= document.createElement("tr");
    title.innerHTML = "<th colspan=\"2\">" + name + "</th>";
    table.appendChild(title);
    /* render the json to HTML by recursively calling print*
     * functions on JSON objects */
    printBase("", struct);
}

/* evaluate statements which creates the object for
 * argument. then the object is converted to JSON
 * and sended to the TAC http handler */
function sendForm(name) {
    var objToSend;
    for (var i = 0; i < evals.length; i++) {
        var evalStr = "objToSend" + evals[i];
        eval(evalStr);
    }
    var response = set("?name=" + name, JSON.stringify(objToSend));
    showForm(name + " returns", JSON.parse(response));
    setNaviRefresh("");
    setNaviBack("showDialog(\"" + name + "\")");
}

/* get json list of provided structures */
var list = JSON.parse(get("?list=1")).list;

/* render the list of functions to html table
 * there is onclick action for each structure in table
 * calling showDialog(functioName) which dynamically redraws
 * the page showing dialog for requested function */
document.write("<div><span>");
document.write("<a id=\"back\"href=\"/\"/>Back</a>");
document.write("</span> | <span>");
document.write("<a id=\"refresh\"href=\"/" + base_uri + "\"/>Refresh</a>");
document.write("</span>");
document.write("<hr/>");
document.write("<table id=\"table\" border=\"1\">");
document.write("<tr><th>function</th><th>plain json</th></tr>");
for (var i = 0; i < list.length; i++) {
    document.write("<tr><td><a href=\"#\" onclick=\"showDialog('" + list[i] + "')\">" + list[i] + "</a></td>");
    document.write("<td><a href=\"?name=" + list[i] + "\">json</a></td></tr>");
}
document.write("</table><div id=\"button\"></div></div>");

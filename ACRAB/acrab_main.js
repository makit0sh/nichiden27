var ip = ['http://192.168.0.199/', 'http://192.168.0.200/'];
var name = ['LED1', 'LED2'];

function pageInit(){
    ip.forEach(function(value){GETRequest(value + 'gpio/0');});
}

function GETRequest(url){
    var xhr= new XMLHttpRequest();
    xhr.addEventListener('loadend', function(){ // xhr.readyStateが4になったら実行される
        if(xhr.status === 200){
            console.log(xhr.response);
            checkStatus(JSON.parse(xhr.response));
        }
        else{
            console.error(xhr.status+' '+xhr.statusText);
            }
        });
    xhr.open("GET", url);
    xhr.send();
}

function checkStatus(stat){
    ['LED1', 'LED2'].forEach(function(value){
        switch(stat[value]){
            case 1: console.log(value + " is on"); break;
            case 0: console.log(value + " is off"); break;
        }
    });
}

function Switch(a,b){
    document.getElementById(b).style.display = "none";
    document.getElementById(a).style.display = "block";
}

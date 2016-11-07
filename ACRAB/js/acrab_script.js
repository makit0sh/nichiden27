var info, scenario = {};
const SCENARIO_COUNT = 2;

(function(){
  for(var i=0;i<SCENARIO_COUNT;i++){ // Initialize select box
    $.when($.getJSON('scenario/'+ i +'.json')).done(function(data){
      info = data.info;
    });
  }
  getScenarioData(0);
  scenarioInit();
}());


function getScenarioData(num){
    $.when($.getJSON('scenario/'+ num +'.json')).done(function(data){
      info = data.info;
      scenario = data.scenario;
    });
}

function scenarioInit(){
  $('#scenario_title').html('<b>'+info.title+'</b> by'+info.name);
  $('#scenario_prev').html('(前のシーンが表示されます)').addClass('scenario0').attr('onclick', 'goPrev();').prop('disabled', true);
  viewScript('#scenario_now', 0);
  $('#scenario_now').addClass('scenario1').prop('disabled', true);
  viewScript('#scenario_next', 1);
  $('#scenario_next').addClass('scenario2').attr('onclick', 'goNext();').prop('disabled', true);
  $('#scenario_number').html('1/' + scenario.length);
}

var timer_button = new function(){
  this.start = function(){
    sendComm(0, 0);
    $('#scenario_next').prop('disabled', false);
    timer = setInterval(function(){pass_time++; readTime();}, 1000);
    $('#timer_start').hide();
    $('#timer_stop').show();
    $('#timer_reset').prop('disabled', true);
  };
  this.stop = function(){
    clearInterval(timer);
    $('#timer_stop').hide();
    $('#timer_restart').show();
    $('#timer_reset').prop('disabled', false);
  };
  this.restart = function(){
    timer = setInterval(function(){pass_time++; readTime();}, 1000);
    $('#timer_restart').hide();
    $('#timer_stop').show();
    $('#timer_reset').prop('disabled', true);
  };
  this.reset = function(){
    pass_time = 0;
    readTime();
    scenarioInit();
    $('#timer_restart').hide();
    $('#timer_start').show();
    $('#timer_reset').prop('disabled', true);
  };
  var pass_time = 0;
  var readTime = function(){
    var hour     = toDoubleDigits(Math.floor(pass_time / 3600));
    var minute   = toDoubleDigits(Math.floor((pass_time - 3600*hour) / 60));
    var second   = toDoubleDigits((pass_time - 3600*hour - 60*minute));
    $('#scenario_timecount').html(hour + ':' + minute + ':' + second);
    return;
  };
  var toDoubleDigits = function(num){return ('0' + num).slice(-2);}; // sliceで時刻要素の0埋め
};

function goNext(){
  $.each(['scenario_prev', 'scenario_now', 'scenario_next'], function(){
    var num = $('#'+this).get(0).className.match(/\d/g).join('') / 1; // 数字だけ取り出して渡す(型変換しないとうまくいかなかった)
    $('#'+this).removeClass($('#'+this).get(0).className).addClass('scenario' + (num+1));
    if(num+1 > scenario.length) $('#'+this).html('(原稿の最後です)').prop('disabled', true);
    else{
      if(this == 'scenario_now') sendComm(num, 0);
      viewScript('#'+this, num);
    }
  });
  $('#scenario_number').html($('#scenario_now').get(0).className.match(/\d/g).join('') + '/' + scenario.length);
}

function goPrev(){
  $.each(['scenario_prev', 'scenario_now', 'scenario_next'], function(){
    var num = $('#'+this).get(0).className.match(/\d/g).join('') / 1; // 数字だけ取り出して渡す(型変換しないとうまくいかなかった)
    $('#'+this).removeClass($('#'+this).get(0).className).addClass('scenario' + (num-1));
    if(num-1 <= 0) $('#'+this).html('(前のシーンが表示されます)').prop('disabled', true);
    else{
      if(this == 'scenario_now') sendComm(num-1, 1);
      viewScript('#'+this, num-2);
    }
  });
  $('#scenario_number').html($('#scenario_now').get(0).className.match(/\d/g).join('') + '/' + scenario.length);
}

function sendComm(index, reverse){
  var data = $.extend(true, {}, scenario[index].projector);
  if(reverse) $.each(data, function(key){
    data[key] = this == 1 ? 0 : 1;
    });
  $.each(ip, function(){
    address = this + 'setPort/status.json';
    getRequest(address, data).done(function(res){checkStatus(res)});
  });
}

function viewScript(id, index){
  if($(id).is(':disabled'))$(id).prop('disabled', false);
  $(id).html(function(){
    var res = ''
    if(!scenario[index].word) res += '開始直後'
    else {
      res += '「'+scenario[index].word+'」の';
      switch(scenario[index].timing){
        case 'pre': res += '前'; break;
        case 'post': res += '後'; break;
        default: res +=  scenario[index].timing; break;
      }
    }
    $.each(scenario[index].projector, function(key){
      res += '<br>' + port[key].name + 'を' + (this == 1 ? '点灯' : '消灯');
    });
    return res;
  });
}


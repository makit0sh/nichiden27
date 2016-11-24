var info, scenario = {};
const SCENARIO_COUNT = 13; // ファイルの数はブラウザからじゃわからないので必ずここで指定!!!

(function(){
  var scenario_file = [];
  /*** Initialize select box ***/
  for(var i=0;i<SCENARIO_COUNT;i++) scenario_file[i] = $.getJSON('scenario/'+ i +'.json');
  $.when.apply($, scenario_file).done(function(){ // シナリオファイルが全部取得できたら<option>と<optgroup>追加
    $.each(arguments, function(index){ // argumentsに取得したjsonが全部入ってるのでそれぞれ読む
      var init_info = this[0].info;
      var $dayGroup = $('#select > optgroup[label='+init_info.day+']'); // <option>を入れる<optgroup>
      if(!$dayGroup[0]){
        $('#select').append($('<optgroup>', {label: init_info.day}));
        $dayGroup = $('#select > optgroup[label='+init_info.day+']');
      }
      $dayGroup.append($('<option>', {
        value: index,
        text: init_info.name + ' -  ' + init_info.title
      }));
    });
    $('select#select').change(function(){
      getScenarioData($(this).val());
    });
  }).fail(function(xhr){console.error(xhr.status+' '+xhr.statusText);});
  getScenarioData(0);
}());

function getScenarioData(num){
  console.debug('getScenarioData called. num: '+num);
  $.when($.getJSON('scenario/'+ num +'.json')).done(function(data){
    info = data.info;
    scenario = data.scenario;
    scenarioInit();
  });
}

function scenarioInit(){
  $('#scenario_prev').html('(前のシーンが表示されます)').addClass('scenario0').attr('onclick', 'goPrev();').prop('disabled', true);
  viewScript('#scenario_now', 0);
  $('#scenario_now').addClass('scenario1').prop('disabled', true);
  viewScript('#scenario_next', 1);
  $('#scenario_next').addClass('scenario2').attr('onclick', 'goNext();').prop('disabled', true);
  $('#scenario_number').html('1/' + scenario.length);
  $('#progress_bar progress').attr('pass_time', '00:00:00');
}

var timer_button = new function(){
  this.start = function(){
    sendComm(0, 0);
    $('#select').prop('disabled', true);
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
    $('#select').prop('disabled', false);
    $('#timer_restart').hide();
    $('#timer_start').show();
    $('#timer_reset').prop('disabled', true);
    $('#scenario_prev').removeClass(function(index, className) {
          return (className.match(/\bscenario\S+/g) || []).join(' ');
    });
    $('#scenario_now').removeClass(function(index, className) {
          return (className.match(/\bscenario\S+/g) || []).join(' ');
    });
    $('#scenario_next').removeClass(function(index, className) {
          return (className.match(/\bscenario\S+/g) || []).join(' ');
    });
  };
  var pass_time = 0;
  var readTime = function(){
    var hour     = toDoubleDigits(Math.floor(pass_time / 3600));
    var minute   = toDoubleDigits(Math.floor((pass_time - 3600*hour) / 60));
    var second   = toDoubleDigits((pass_time - 3600*hour - 60*minute));
    $('#progress_bar progress').attr('pass_time', hour + ':' + minute + ':' + second);
    var progress = Math.min(pass_time / 1500.0, 1); // 25 minutes
    $('#progress_bar progress').attr('value', progress);
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
    sliced_data = each_slice(data, 5);
    $.each(sliced_data, function(){
      getRequest(address, this).done(function(res){checkStatus(res)});
      sleep_ms(100);
    });
  });
}

function viewScript(id, index){
  if($(id).is(':disabled'))$(id).prop('disabled', false);
  $(id).html(function(){
    var res = ''
    if(!scenario[index].word) res += '開始前'
    else {
      res += '「'+scenario[index].word+'」の';
      switch(scenario[index].timing){
        case 'pre': res += '言い始め'; break;
        case 'post': res += '言い終り'; break;
        default: res +=  scenario[index].timing; break;
      }
    }
    res += '<br>';
    $.each(scenario[index].projector, function(key,index){
      res += '[' +port[key].name + (this == 1 ? '点灯' : '消灯') + '] ';
    });
    return res;
  });
}


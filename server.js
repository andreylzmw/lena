const path = require('path');
const { exec } = require("child_process");
var bodyParser = require('body-parser');
var express = require('express');

var app = express();

app.use(express.static(__dirname + '/'));
// app.use(bodyParser.urlencoded({extend:true}));
app.engine('html', require('ejs').renderFile);
app.set('view engine', 'ejs');
app.set('views', __dirname);

const port = 3000;

app.get('/', function(req, res){
    res.render("index", {data: ''});
});

app.post('/', function (req, res) {
    var data;
    exec("./metrics", (error, stdout, stderr) => {
        data=stdout.toString();
        console.log(data);
        res.render("index", {data: data});
    });
    
});

app.listen(port, () => console.log(`Running on port ${port}`))
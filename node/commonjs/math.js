// math.js
exports.add = function () {
    for (sum = 0, i = 0, l = arguments.length; i < l; i++) {
        sum += arguments[i];
    }
    return sum;
};



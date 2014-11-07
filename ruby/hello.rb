# Output "I love Ruby"
say = "I love Ruby"
puts say
#
# # Output "I *LOVE* RUBY"
say['love'] = "*love*"
puts say.upcase
#
# # Output "I *love* Ruby"
# # five times
5.times { puts say }

class DBRepo
    attr_reader :content_dir
    @content_dir = folder
end

def find_post(entry_id)
    result = nil
    each_post { |post|
        result = post
        break if post.entry_id = entry_id
    }
    result
end


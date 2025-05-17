use std::borrow::BorrowMut;
use std::fmt;
use std::ops;
use std::ops::Not;

use crate::*;
use nom::FindSubstring;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Index(u64);

impl Index {
    pub fn new(i: u64) -> Index {
        Index(i)
    }

    pub fn value(&self) -> u64 {
        self.0
    }
}

impl ops::AddAssign<u64> for Index {
    fn add_assign(&mut self, rhs: u64) {
        if rhs > 1 {
            panic!("Didn't expect to increment by more than one!");
        }
        self.0 += rhs
    }
}

impl fmt::Display for Index {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.0)
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Description(String);

impl Description {
    pub fn new(s: &str) -> Description {
        Description(s.to_owned())
    }

    pub fn value(&self) -> &str {
        &self.0
    }
}

impl fmt::Display for Description {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.0)
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Tag(String);

impl Tag {
    pub fn new(s: &str) -> Tag {
        Tag(s.to_owned())
    }

    pub fn value(&self) -> &str {
        &self.0
    }

    pub fn from_strings(ss: Vec<&str>) -> Vec<Tag> {
        ss.clone().into_iter().map(|s| Tag::new(s)).collect()
    }
}

impl fmt::Display for Tag {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.0)
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct TodoItem {
    pub index: Index,
    pub description: Description,
    pub tags: Vec<Tag>,
    pub done: bool,
}

impl TodoItem {
    pub fn new(index: Index, description: Description, tags: Vec<Tag>, done: bool) -> TodoItem {
        TodoItem {
            index,
            description,
            tags,
            done,
        }
    }
}

impl fmt::Display for TodoItem {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{} {} ", self.index, self.description)?;
        for tag in &self.tags {
            write!(f, "#{} ", tag)?;
        }
        Ok(())
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct TodoList {
    top_index: Index,
    // deque has faster mid deletions
    // items: VecDeque<TodoItem>,
    items: Vec<TodoItem>,
}

impl TodoList {
    pub fn new() -> TodoList {
        TodoList {
            top_index: Index::new(0),
            // items: VecDeque::from([]),
            items: vec![],
        }
    }

    pub fn push(&mut self, description: Description, tags: Vec<Tag>) -> &TodoItem {
        let item = TodoItem {
            index: self.top_index,
            description,
            tags,
            done: false,
        };
        // self.items.push_back(item); // for Deque
        self.items.push(item);
        self.top_index += 1;
        &self.items.last().unwrap()
    }

    pub fn done_with_index(&mut self, idx: Index) -> Option<Index> {
        if let Ok(found_idx) = self.items.binary_search_by(|item| item.index.0.cmp(&idx.0)) {
            self.items[found_idx].done = true;
            // discards the item (for Deque):
            // Some(self.items.remove(found_idx).unwrap().index)
            Some(self.items[found_idx].index.clone())
        } else {
            None
        }
    }

    #[inline]
    fn subsequence(string: &str, subseq: &str) -> bool {
        let mut subseq_chars = subseq.chars();
        let mut current_subseq_char = subseq_chars.next();

        for s_char in string.chars() {
            if let Some(subseq_char) = current_subseq_char {
                if s_char == subseq_char {
                    current_subseq_char = subseq_chars.next();
                }
                if current_subseq_char.is_none() {
                    return true;
                }
            }
        }

        current_subseq_char.is_none()
    }

    /// each word is searched in description separetely for sub-/strings
    #[inline]
    fn match_desc(description: &Description, search_words: &Vec<SearchWord>) -> bool {
        search_words.iter().all(|search_word| {
            description
                .value()
                .split_whitespace()
                .any(|word| Self::subsequence(word, search_word.0.as_str()))
        })
    }

    /// for each searched tag, match partial tags.
    /// @TODO All tags must have a match?
    #[inline]
    fn match_tags(tags: &Vec<Tag>, search_tags: &Vec<Tag>) -> bool {
        search_tags.iter().all(|search_tag| {
            tags.iter()
                .any(|tag| Self::subsequence(tag.0.as_str(), search_tag.0.as_str()))
        })
    }

    pub fn search(&self, sp: SearchParams) -> Vec<&TodoItem> {
        let ignore_desc = sp.words.is_empty();
        let ignore_tags = sp.tags.is_empty();
        if ignore_desc && ignore_tags {
            // TODO: if no search params? (query should fail...)
            return vec![];
        }
        self.items
            .iter()
            .filter(|TodoItem { done, .. }| done.not())
            .filter(|TodoItem { description, .. }| {
                ignore_desc || Self::match_desc(description, &sp.words)
            })
            .filter(|TodoItem { tags, .. }| ignore_tags || Self::match_tags(tags, &sp.tags))
            .collect()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn index_impl_tests() {
        let idx = Index::new(42);
        assert_eq!(idx.value(), 42);
    }

    #[test]
    fn index_add_assign_tests() {
        let mut idx = Index::new(42);
        idx += 1;
        assert_eq!(idx.value(), 43);
        let result = std::panic::catch_unwind(|| {
            let mut idx = Index::new(42);
            idx += 2
        });
        assert!(result.is_err());
    }

    // I'd have preferred consts, but as tags depends on String (dynamic type),
    // then they can't declare consts like this:
    // const TAGS_1 = vec![Tag("bbq".to_string()), Tag("summer".to_string())];
    fn tags1() -> Vec<Tag> {
        vec![Tag::new("bbq"), Tag::new("summer")]
    }
    fn tags2() -> Vec<Tag> {
        vec![Tag::new("winter")]
    }
    fn tl_with_two_items() -> TodoList {
        let mut tl = TodoList::new();
        tl.push(Description(String::from("goal item1")), tags1());
        tl.push(Description(String::from("goal item2")), tags2());
        tl
    }

    #[test]
    fn tl_top_index() {
        assert_eq!(tl_with_two_items().top_index.value(), 2);
    }

    #[test]
    fn tl_search_words1() {
        let tl = tl_with_two_items();
        let tl1_found = tl.search(SearchParams {
            words: vec![SearchWord::new("tm")],
            tags: vec![],
        });
        assert_eq!(tl1_found.len(), 2);
    }

    #[test]
    fn tl_search_words2() {
        let tl = tl_with_two_items();
        let tl2_found = tl.search(SearchParams {
            words: vec![SearchWord::new("gal")],
            tags: vec![],
        });
        assert_eq!(tl2_found.len(), 2);
    }

    #[test]
    fn tl_search_words3() {
        let tl = tl_with_two_items();
        let tl3_found = tl.search(SearchParams {
            words: vec![SearchWord::new("item3")],
            tags: vec![Tag::new("bb")],
        });
        assert_eq!(tl3_found.len(), 0);
    }

    #[test]
    fn tl_search_tags() {
        let tl = tl_with_two_items();
        let tl_found = tl.search(SearchParams {
            words: vec![],
            tags: vec![Tag::new("winter")],
        });
        assert_eq!(tl_found.len(), 1);
    }
    #[test]
    fn tl_search_words_and_tags() {
        let tl = tl_with_two_items();
        let tl_found = tl.search(SearchParams {
            words: vec![SearchWord::new("tem")],
            tags: vec![Tag::new("summer")],
        });
        assert_eq!(tl_found.len(), 1);
    }

    #[test]
    fn tl_done() {
        let mut tl = tl_with_two_items();
        assert!(tl
            .done_with_index(Index::new(1))
            .is_some_and(|idx| idx.0 == 1));
        assert!(tl
            .done_with_index(Index::new(0))
            .is_some_and(|idx| idx.0 == 0));
        assert_eq!(
            tl.search(SearchParams {
                words: vec![SearchWord::new("tem")],
                tags: vec![],
            })
            .len(),
            0
        );
        tl.push(Description(String::from("goal item3")), vec![]);
        assert_eq!(tl.items.len(), 3);
    }
}

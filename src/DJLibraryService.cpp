#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>

DJLibraryService::DJLibraryService(const Playlist &playlist)
    : playlist(playlist), library() {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */

DJLibraryService::~DJLibraryService()
{
    for (AudioTrack *track : library)
    {
        delete track;
    }
    library.clear();
}
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo> &library_tracks)
{
    // Todo: Implement buildLibrary method
    int count = 0;
    for (SessionConfig::TrackInfo track_info : library_tracks)
    {
        if (track_info.type == "MP3")
        {
            AudioTrack *mp3_track = new MP3Track(track_info.title,
                                                 track_info.artists,
                                                 track_info.duration_seconds,
                                                 track_info.bpm,
                                                 track_info.extra_param1, // bitrate
                                                 track_info.extra_param2  // has_tags
            );
            count++;
            library.push_back(mp3_track);
            std::cout << "MP3Track created: " << track_info.extra_param1 << " kbps" << std::endl;
        }
        else if (track_info.type == "WAV")
        {
            AudioTrack *wav_track = new WAVTrack(track_info.title,
                                                 track_info.artists,
                                                 track_info.duration_seconds,
                                                 track_info.bpm,
                                                 track_info.extra_param1, // sample_rate
                                                 track_info.extra_param2  // bit_depth
            );
            count++;
            library.push_back(wav_track);
            std::cout << "WAVTrack created: " << track_info.extra_param1 << "Hz/" << track_info.extra_param2 << "bit" << std::endl;
        }
    }
    std::cout << "[INFO] Track library built: " << count << " tracks loaded" << std::endl;
}

/**
 * @brief Display the current state of the DJ library playlist
 *
 */
void DJLibraryService::displayLibrary() const
{
    std::cout << "=== DJ Library Playlist: "
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty())
    {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 *
 * @return Playlist&
 */
Playlist &DJLibraryService::getPlaylist()
{
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 *
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack *DJLibraryService::findTrack(const std::string &track_title)
{
    // Your implementation here
    AudioTrack *found_track = playlist.find_track(track_title);
    return found_track;
}

void DJLibraryService::loadPlaylistFromIndices(const std::string &playlist_name,
                                               const std::vector<int> &track_indices)
{
    // Your implementation here
    int count = 0;
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;
    Playlist new_playlist(playlist_name);
    this->playlist = new_playlist;
    for (int index : track_indices)
    {
        if (index < 1 || static_cast<size_t>(index) > library.size())
        {
            std::cout << "[WARNING] Invalid track index: " << index << std::endl;
            continue;
        }
        else
        {
            AudioTrack *track_to_add = library[index - 1];
            PointerWrapper<AudioTrack> track_clone = track_to_add->clone();
            AudioTrack *track_ptr = track_clone.release();
            if (track_ptr == nullptr)
            {
                std::cout << "[ERROR] Failed to clone track: " << track_to_add->get_title() << std::endl;
                continue;
            }
            track_ptr->load();
            track_ptr->analyze_beatgrid();
            new_playlist.add_track(track_ptr);
            count++;
            std::cout << "Added '" << track_ptr->get_title() << "' to playlist '" << playlist_name << "'" << std::endl;
        }
    }
    this->playlist = std::move(new_playlist);
    std::cout << "[INFO] Playlist loaded: " << playlist_name << " (" << count << " tracks)" << std::endl;
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const
{
    // Your implementation here
    std::vector<std::string> titles;
    for (AudioTrack *track : playlist.getTracks())
    {
        titles.push_back(track->get_title());
    }
    return titles;
}
